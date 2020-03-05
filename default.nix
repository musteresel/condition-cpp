with import <nixpkgs> {};

let condition-cpp = import ./condition-cpp.nix;
in
{
  with-clang = callPackage condition-cpp {stdenv = clangStdenv;};
  with-gcc = callPackage condition-cpp {};
}
