# A Compiler for the Oberon-0 Programming Language
Text processing, programming language semantics, data structures, optimization problems and under-the-hood assembly shenagigans - all of these Computer Science microcosms come together in the form of a compiler which is exactly the reason why many aspiring C.S. students stumble into the fever dream that building an own compiler ensues. 

## About Oberon-0
This project implements a compiler for Oberon-0, a relatively rudementary subset of the [Oberon](https://oberon.org/en) programming language created by [Niklaus Wirth](https://people.inf.ethz.ch/wirth/) (who was also the inventor of Pascal, Modula-2 and contributor of many more PLs which is why he eventually won the Turing Award in 1984). Oberon is a very strongly typed imperative language with a high focus on modularity and simplicity. Right now, this compiler only supports Oberon-0 (which omits more "complex" elements like pointers, strings and procedure variables), but a more extensive covering of full Oberon is aspired.

## About this project
The compiler was originally built as part of the M.Sc. course "Compiler Construction" under [Michael Grossniklaus](https://dbis.uni-konstanz.de/people/people/grossniklaus/) at the [University of Constance](https://www.uni-konstanz.de/en/) together with fellow student [Elias Maier](https://github.com/eelias13). Technically, it's only a compiler-frontend which transforms high-level Oberon-0 source code into the low-level, assembly-like [LLVM-Intermediate-Representation](https://llvm.org/) (which has the added convenience that compiled programs can be easily linked and executed using [clang](https://clang.llvm.org/)). In uploading and continuing my work on this project, I aim at demonstrating the development of compilers from a learning-based perspective. If you are (for whatever reason :p) looking for a more extensive compiler, refer to [Michael Grossniklaus' pet project](https://github.com/zaskar9/oberon-lang).

## Dependencies
This code depends on [LLVM](https://llvm.org/) and the following [Boost](https://www.boost.org/) libraries: `system`,`convert`,`filesystem`, `program_options`. Both have to be installed on a host-machine to build the compiler. Ideally, a package manager can be used. For example,
 * [vcpkg]() for Windows (Note that the LLVM download will be **quite** large. Maybe you might want to refer to the official [LLVM website's instructions](https://releases.llvm.org/download.html))
 * [Homebrew](https://brew.sh/) for MacOS
 * [APT](https://wiki.debian.org/Apt) for Linux

## How to build
The project must be built from source. Both CMake and a C++ compiler supporting the C++17 standard are required. If these requirements are statisfied, then the `CMakeList.txt` file handles the building of the project. Additionally, the project may also be built with Docker using the `Dockerfile`, but expect limited support.

## Authors and acknowledgement
The initial skeleton for the project and the scanner used were both written by [Michael Grossniklaus](https://dbis.uni-konstanz.de/people/people/grossniklaus/) for the previously mentioned M.Sc. course. Major parts of the initial code generation segments and a lot of supporting code of other sections were written by [Elias Maier](https://github.com/eelias13).

The majority of the project and all it's future updates are written by Max Galetskiy.

