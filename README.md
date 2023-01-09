# dwarflection

Adding realtime to C via [DWARF debugging data](https://dwarfstd.org/).

Inspired by:
- Jack Wrenn's [Deflect](https://jack.wrenn.fyi/blog/deflect/) library for Rust
- jdh's [Archimedes](https://www.youtube.com/watch?v=aJt2POa9oCM) library for C++
	- He ultimately decided not to use debugging data since C++ is too complicated; C (kinda) doesn't have that problem!

This repo has also kinda devolved into messing around with C and its preprocessor,
but it's all in service of the mission.

## Install and Run
On Ubuntu:
```bash
sudo apt install dwarfdump libdwarf-dev
./compile
```

## TODO
- Implement a procmaps parser with array lists
- Begin wrapping libdwarf
- 
- Beat [address space layout randomization](https://en.wikipedia.org/wiki/Address_space_layout_randomization)
	- [Arenas](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)
