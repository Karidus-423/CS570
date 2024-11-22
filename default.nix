{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "sun-file";
  src = ./src;

  # Copy source to the build directory
  unpackPhase = ''
    cp -r $src/* .
  '';

  # Build in the writable build directory
  buildPhase = ''
    mkdir -p bin obj
    make
  '';

  # Install the built binaries
  installPhase = ''
    mkdir -p $out/bin
    cp bin/* $out/bin
  '';

  # Clean up the build directory
  cleanPhase = ''
    make clean
  '';

  # Dependencies for building
  nativeBuildInputs = [ pkgs.gcc ];
  buildInputs = [ pkgs.ncurses pkgs.libtirpc pkgs.glibc ];
}

