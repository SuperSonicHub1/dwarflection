// https://www.prevanders.net/libdwarfdoc/group__initfunctions.html#gac3793e6b9e78d0cd8af1e80ced0dfef9

#include <libdwarf/dwarf.h>
#include <libdwarf/libdwarf.h>
#include <stdbool.h>
#include <stddef.h> // Everyone needs a little NULL in their lives.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://github.com/jswrenn/deflect/blob/b7c60ce053d5d2b29957e848ed902d702f461017/src/lib.rs#L161-L176
// 7ffc7b341000-7ffc7b343000 r-xp 00000000 00:00 0 [vdso]
static long get_relative_address(long ptr) {
  long start_address, end_address, offset, result;

  FILE *file;
  char *line;
  size_t len = 0;
  ssize_t read;

  file = fopen("/proc/self/maps", "r");
  if (file == NULL) {
    printf("Something is horribly wrong with Linux.");
    return -1;
  }

  while ((read = getline(&line, &len, file)) != -1) {
    char *line_ptr;
    // Read start
    start_address = strtol(line, &line_ptr, 16);
    // Skip dash
    line_ptr++;
    end_address = strtol(line_ptr, &line_ptr, 16);
    // Skip permissions
    line_ptr += 6;
    offset = strtol(line_ptr, NULL, 16);

    if (start_address <= ptr && ptr < end_address) {
      result = ptr - start_address + offset;
    }
  }

  free(line);
  return result;
}

static Dwarf_Debug debug_info = 0;
static Dwarf_Error loading_error = 0;

int dwarflection_init() {
  return dwarf_init_path(
      // https://stackoverflow.com/questions/1528298/get-path-of-executable
      "/proc/self/exe", NULL, 0, DW_GROUPNUMBER_ANY,
      // Incompatible pointer to integer conversion passing 'void *' to
      // parameter of type 'unsigned int'. Unsigned int? I thought passsing NULL
      // was good! > Pass in NULL...
      NULL, NULL, &debug_info, &loading_error,
      // I think internal APIs here are leaking. This isn't in the docs.
      // https://www.prevanders.net/libdwarfdoc/group__initfunctions.html#gac3793e6b9e78d0cd8af1e80ced0dfef9
      NULL, 0, 0, NULL);
}

static Dwarf_Attribute get_location_attr(Dwarf_Die dw_die,
                                         Dwarf_Error *dw_error) {
  Dwarf_Attribute attr;
  dwarf_attr(dw_die, DW_AT_location, &attr, dw_error);
  return attr;
}

static char *parse_dies_for_type(long ptr, Dwarf_Die in_die, int is_info,
                                 int in_level) {
  int res = DW_DLV_OK;
  Dwarf_Die cur_die = in_die;
  Dwarf_Die child = 0;
  Dwarf_Error error = 0;
  Dwarf_Attribute attr;

  // TODO: Check here
  attr = get_location_attr(cur_die, res);
  if (res == DW_DLV_ERROR) {
      printf("Problems getting attr\n");
      exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_OK) printf("Attr: ", attr);

  /*   Loop on a list of siblings */
  for (;;) {
    Dwarf_Die sib_die = 0;

    /*  Depending on your goals, the in_level,
        and the DW_TAG of cur_die, you may want
        to skip the dwarf_child call. */
    res = dwarf_child(cur_die, &child, &error);
    if (res == DW_DLV_ERROR) {
      printf("Error in dwarf_child , level %d \n", in_level);
      exit(EXIT_FAILURE);
    }
    if (res == DW_DLV_OK) {
      return parse_dies_for_type(ptr, child, is_info, in_level + 1);
      /* No longer need 'child' die. */
      dwarf_dealloc(debug_info, child, DW_DLA_DIE);
      child = 0;
    }
    /* res == DW_DLV_NO_ENTRY or DW_DLV_OK */
    res = dwarf_siblingof_b(debug_info, cur_die, is_info, &sib_die, &error);
    if (res == DW_DLV_ERROR) {
      exit(EXIT_FAILURE);
    }
    if (res == DW_DLV_NO_ENTRY) {
      /* Done at this level. */
      break;
    }
    /* res == DW_DLV_OK */
    if (cur_die != in_die) {
      dwarf_dealloc(debug_info, cur_die, DW_DLA_DIE);
      cur_die = 0;
    }
    cur_die = sib_die;
    // TODO: Check here
  }
}

char *dwarflection_get_type(long ptr) {
  long relative_addr = get_relative_address(ptr);

  // Get .debug_info
  // https://www.prevanders.net/libdwarfdoc/group__examplecuhdr.html
  Dwarf_Unsigned abbrev_offset = 0;
  Dwarf_Half address_size = 0;
  Dwarf_Half version_stamp = 0;
  Dwarf_Half offset_size = 0;
  Dwarf_Half extension_size = 0;
  Dwarf_Sig8 signature;
  Dwarf_Unsigned typeoffset = 0;
  Dwarf_Unsigned next_cu_header = 0;
  Dwarf_Half header_cu_type = 0;
  Dwarf_Bool is_info = 1; // Originally TRUE, but TRUE isn't in scope
  int header_res = 0;
  Dwarf_Error error = 0;

  while (true) {
    Dwarf_Die no_die = 0;
    Dwarf_Die cu_die = 0;
    Dwarf_Unsigned cu_header_length = 0;

    memset(&signature, 0, sizeof(signature));
    header_res = dwarf_next_cu_header_d(
        debug_info, is_info, &cu_header_length, &version_stamp, &abbrev_offset,
        &address_size, &offset_size, &extension_size, &signature, &typeoffset,
        &next_cu_header, &header_cu_type, &error);

    // Only care about .debug_info
    if (header_res == DW_DLV_NO_ENTRY) {
      /*  No more CUs to read! Never found
          what we were looking for in either
          .debug_info or .debug_types. */
      return "";
    }
    /* The CU will have a single sibling, a cu_die. */
    header_res =
        dwarf_siblingof_b(debug_info, no_die, is_info, &cu_die, &error);
    if (header_res == DW_DLV_ERROR) {
      return "";
    }
    if (header_res == DW_DLV_NO_ENTRY) {
      /*  Impossible */
      exit(EXIT_FAILURE);
    }

    // Loop over DIEs
    return parse_dies_for_type(ptr, cu_die, is_info, 0);
  }
}

void dwarflection_close() { dwarf_finish(debug_info, &loading_error); }
