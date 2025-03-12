# A Compiler for the Oberon-0 Programming Language
Text processing, programming language semantics, data structures, optimization problems and under-the-hood assembly shenagigans - all of these Computer Science microcosms come together in the form of a compiler which is exactly the reason why many aspiring C.S. students stumble into the fever dream that building an own compiler ensues. 

## About Oberon-0
This project implements a compiler for Oberon-0, a relative rudementary subset of the [Oberon](https://oberon.org/en) programming language created by [Niklaus Wirth](https://people.inf.ethz.ch/wirth/) (who was also the inventor of Pascal, Modula-2 and contributor of many more PLs which is why he eventually won the Turing Award in 1984). Oberon is a very strongly typed imperative language with a high focus on modularity and simplicity. Right now, this compiler only supports Oberon-0 (which omits more "complex" elements like pointers, strings and procedure variables), but a more extensive covering of full Oberon is aspired.

## About this project
The compiler was originally built as part of the M.Sc. course "Compiler Construction" under [Michael Grossniklaus](https://dbis.uni-konstanz.de/people/people/grossniklaus/) at the [University of Constance](https://www.uni-konstanz.de/en/) together with fellow student [Elias Maier](https://github.com/eelias13). Technically, it's only a compiler-frontend which transforms high-level Oberon-0 source code into the low-level, assembly-like [LLVM-Intermediate-Representation](https://llvm.org/) (which has the added convenience that compiled programs can be easily linked and executed using [clang](https://clang.llvm.org/)). In uploading and continuing my work on this project, I aim at demonstrating the development of compilers from a learning-based perspective. If you are (for whatever reason :p) looking for a more extensive compiler, refer to [Michael Grossniklaus' pet project](https://github.com/zaskar9/oberon-lang).

## Dependencies
tba

## How to build
tba

## Authors and acknowledgement
tba 
