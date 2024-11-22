{
  description = "CS570 Flake";

  # Flake inputs
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    old-glibc-nixpkgs.url = "github:nixos/nixpkgs/83162ab3b97d0e13b08e28938133381a7515c1e3";
  };

  # Flake outputs
  outputs = { self, nixpkgs, flake-utils, old-glibc-nixpkgs }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = nixpkgs.legacyPackages.${system};
      glibc2_31 = old-glibc-nixpkgs.legacyPackages.${system}.glibc;
    in
    {
      # Development shell
      devShells.default = pkgs.mkShell {
        name = "CS570-OS";
        buildInputs = with pkgs; [
          clang-tools
          rpcsvc-proto
          ntirpc
          rpcbind
          glibc
          libtirpc
          ncurses
          gdb
          libnsl
          glibc2_31
          readline
        ];
      };

      # Package definition
      packages.default = pkgs.stdenv.mkDerivation {
        name = "sun-server";
        src = ./src;

        # Copy source to the build directory
        unpackPhase = ''
          cp -r $src/* .
        '';

        # Build in the writable build directory
		# mkdir -p bin obj
        buildPhase = ''
          make all
        '';

        # Install the built binaries
		installPhase = ''
		  mkdir -p $out/bin
		  cp client $out/bin/
		  cp server $out/bin/
		'';

        # Clean up the build directory
        cleanPhase = ''
          make clean
        '';

        # Dependencies for building
        nativeBuildInputs = [ pkgs.gcc ];
        buildInputs = [ pkgs.ncurses pkgs.libtirpc pkgs.glibc glibc2_31];
      };
    });
}

