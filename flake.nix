{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";

    flake-utils = {
      inputs.nixpkgs.follows = "nixpkgs";
      url = "github:numtide/flake-utils";
    };
  };

  outputs = { self, nixpkgs, flake-utils, ... }@inputs:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
        rec {
          defaultPackage = packages.lua;
          packages.lua = pkgs.callPackage ./package.nix {};

          plugin = ''${packages.lua}/lib/libNix.Lua.so'';
          loader = import ./implement.nix {
            lua = lua_lib;
          };
        }
    );
}
