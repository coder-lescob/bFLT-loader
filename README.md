# bFLT-loader

Loads and run a program from an flat binary file (bFLT). For now support is only available for x86_64 (and I'm not even sure it works 100%). For now programs that places the data section before the code section are rejected, support may be added later.

## how to run

Yet the support is very experimental. Tho you can run it by first cloning this repo then by running `make run ARGS=path/to/flat/binary`. The repo includes some examples (1 for now),

```sh
make run ARGS=tests/hello.bin
```

will run the hello, world program that I wrote by hand.

## contribute

Contributions are welcome.
