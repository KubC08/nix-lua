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

### Use case examples
To set your computer's network hostName you can use this code.
This is equivalent to ``networking.hostName = "test-pc";`` in Nix.
```nix
(builtins.fromLUA ''return { ["networking.hostName"] = "test-pc" }'')
```

You can also use the deep object format such as. Which results in the same output.
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

</br>

## To-do
There are a few things to still be implemented before this library is fully complete. Here's a few

 - [ ] Implement calling Nix functions from within Lua
 - [ ] Implement calling Lua functions from within Nix
 - [ ] Implement optionally returning a Lua function instead of an object
 - [ ] Add direct Lua file imports instead of having to use readFile and fromLUA
 - [ ] Implement Nix imports working with Lua files *(not sure if this one is possible)*
