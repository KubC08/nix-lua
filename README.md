# Nix-Lua
Unlock the power of Lua inside NixOS and Nix manager. Use a more familiar language to write your config files instead.

</br>

## Install
Currently the only supported method of installation is through flakes, this will be changed once the project is fully complete.

1. Open your primary flake.nix file
2. Inside your flake file add the following to the inputs list.
```nix
lua = {
  url = "github:KubC08/nix-lua";
  inputs.nixpkgs.follows = "nixpkgs";
};
```
3. Within the outputs modules add the following.
```nix
inputs.lua.loader.x86_64-linux
```
4. Now you can use builtins.fromLUA

</br>

Example of how your flake.nix file should look like:
```nix
{
  description = "Nixos config flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

    lua = {
      url = "path:/etc/nixos/flakes/modules/nix-lua";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, ... }@inputs: {
    nixosConfigurations.default = nixpkgs.lib.nixosSystem {
      specialArgs = {inherit inputs;};
      modules = [
        ./configuration.nix
        inputs.lua.loader.x86_64-linux
      ];
    };
  };
}
```

> **Already have existing plugins?** You can use ``inputs.lua.lua_lib.x86_64-linux`` for a direct path to the plugin file.

</br>

## How to use
The package adds a simple function called ``builtins.fromLUA`` which takes the given Lua string and executes it, with the returning object being in Nix format.
This can be used to run Lua which returns any given Nix configuration. (Yes you can also use the require function within Lua)

### Use case examples
Here is a simple example of how to set your computer's host name using Lua. This is a simple example however you can set any configuration in there, as long as it is also settable in Nix.
This is equivalent to ``networking.hostName = "test-pc";`` in Nix.
```nix
(builtins.fromLUA ''return { ["networking.hostName"] = "test-pc" }'')
```

You can also use the deep object format such as the given example below. Which results in the same output.
```nix
(builtins.fromLUA ''return { networking = { hostName = "test-pc" } }'')
```
> **Note:** These can be used interchangeably.

### Running from a file
Running from a file is quite simple, and is very similar to how you would run a TOML configuration file. Here's an example:
```nix
(builtins.fromLUA (builtins.readFile ./test.lua))
```
This would execute the ``test.lua`` file and return it's contents as a nix object.

### Running from a Nix import
You can also run Lua from a Nix import and return the given data to the calling Nix file. Here is a simple example:
```nix
imports = [
  (builtins.fromLUA (builtins.readFile ./test.lua))
];
```

</br>

## Building
You can build the project yourself in 2 ways, the easiest would be to simply execute ``nix build`` in the root directory (containing flake.nix).
The alternative would be to build it manually, here's a simple guide to do so:

1. Move to the nix-lua directory.
2. Populate the external directory based on how it's specified in the README.
3. Populate the lib directory based on how it's specified in the README.
4. Execute the command ``premake5 gmake2``.
5. Run the command ``make config=release_linux``

There are also other configurations for building, these are:
- release_linux = Linux x64 Release
- debug_linux = Linux x664 Debug
- release_windows = Windows x64 Release
- debug_windows = Windows x64 Debug
- release_mac = Mac Universal Release
- debug_mac = Mac Universal Debug
- release_macarm = Mac ARM Release
- debug_macarm = Mac ARM Debug
- release_mac64 = Mac x64 Release
- debug_mac64 = Mac x64 Debug

</br>

## To-do
There are a few things to still be implemented before this library is fully complete. Here's a few

 - [ ] Implement calling Nix functions from within Lua
 - [X] Implement calling Lua functions from within Nix
 - [X] Implement optionally returning a Lua function instead of an object
 - [ ] Add direct Lua file imports instead of having to use readFile and fromLUA
 - [ ] Implement Nix imports working with Lua files *(not sure if this one is possible)*

## Credits
- [Lua](https://www.lua.org/) for being able to use the Lua language
- [Nix](https://nixos.org/) for being able to extend Nix
- [Sol](https://github.com/ThePhD/sol2) for being able to use the wrapper for the Lua language
