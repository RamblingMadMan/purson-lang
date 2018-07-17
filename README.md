![Purson 
Logo](https://image.ibb.co/ng9iGo/purson_Lion_Red_Very_Small.png)

[![Build Status](https://travis-ci.com/RamblingMadMan/purson-lang.svg?branch=master)](https://travis-ci.com/RamblingMadMan/purson-lang)

# Purson
Strongly typed functional general purpose programming language.

## Brief
Purson was created out of interest and curiosity rather than a need and as such 
features are added on a very loose schedule. All development is done in a 
rolling release fashion, releases are snapshots of development.

The language is designed to be quick to write, beautiful to look at and easy to do right.

As an example here are a couple of function definitions:

```
fn pow(x, n) => x * pow(x, n-1);
fn pow(x, 1) => x;
fn pow(x, 0) => 1;

fn fib(a) => fib(a-1) + fib(a-2);
fn fib(1) => 1;
fn fib(0) => 0;
```

### Language Features

- Type inference
- Pattern matching
- Automatic differentiation
- Dynamic code generation (self-modification)

## Components

### Purson Library
This library contains functions for lexing, parsing and compiling Purson code as well as utilities to interface with programs written in purson.

Here is a list of completed library features:

- [X] Lexer
- [X] Parser
- [ ] Compiler
- [ ] Type system
- [ ] Module system

### Purson REPL
A REPL environment for testing and prototyping. GNU Readline is used for 
autocompletion and history.

## Dependencies

A few dependencies are required to build/run the Purson components.

### Libraries

* GMP
* MPFR
* ICU
* Readline
* LLVM

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
