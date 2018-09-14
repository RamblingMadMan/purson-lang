<a href="http://purson.io/"><img src="http://purson.io/img/lions-head.png" alt="drawing" width="128"/></a>

# Purson
Strongly typed, functional, general purpose programming language.

## Brief
Purson was created out of interest and curiosity rather than a need; and as such 
features are added on a very irregular basis. All development is done on a loose
rolling release schedule.

The language is designed to be quick to write, beautiful to look at and easy to understand.

Here is a variation of the classic Hello World example:

```
let IO = import("IO");

fn main() => IO.output("Hello, " + IO.input() + "!");
```

## Language Features

- Type inference
- Pattern matching
- Automatic differentiation
- Dynamic code generation

## Components

### Bear
Highly unfinished IDE for Purson projects.

To test, when it opens up point it to the `purson/testing/Main` directory.

![Bear Screenshot](http://purson.io/img/bearEarly.png)

#### Features

- [X] Syntax Highlighting 
- [X] File loading/saving
- [ ] Project handling

### Purson Library
This library contains functions for lexing, parsing and compiling Purson code as well as utilities to interface with programs written in purson.

#### Features

- [X] Lexer
- [X] Parser
- [ ] Compiler
- [ ] Type system
- [ ] Module system

### Purson REPL
A REPL environment for testing and prototyping. GNU Readline is used for 
autocompletion and history.
  
Expressions are treated as if written in the body of a procedural function like so:

```
fn repl(){
	// > 
}
```

Then the value result of the expression is printed.

## Dependencies

A few dependencies are required to build/run the Purson components.

### Required Libraries

* GMP
* MPFR
* ICU
* Readline
* LLVM

### Additional Bear Libraries

* Qt 5

## Building

Purson was mainly developed on a linux box so instructions follow as such, but all commands have windows alternatives.

A simple build will go something like this:

```bash
git submodule update --init --
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

Then to run the REPL:
```bash
./repl/purson-repl
```
