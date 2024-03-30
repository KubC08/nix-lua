{
	lib,
	stdenv,
	premake5,
	gcc,
	gnumake,
	lua,
	nix,
	sol2,
	boost,
}:

stdenv.mkDerivation {
	pname = "lua-bin";
	version = "1.0.0";
	src = ./nix-lua;

	nativeBuildInputs = [ premake5 gcc gnumake ];
	buildInputs = [ lua sol2 nix boost ];
	propagatedBuildInputs = [ nix ];

	preBuild = ''
		# Temporary fix for nix compile error (https://github.com/NixOS/nixpkgs/issues/300092)
		export NIX_CFLAGS_COMPILE="$NIX_CFLAGS_COMPILE -isystem $propagatedBuildInputs/include/nix"
	'';
	makeFlags = [ "config=release_linux" ];
	installPhase = ''
		mkdir -p $out/lib
		cp bin/Release/Linux/libNix.Lua.so $out/lib
	'';

	meta = with lib; {
		description = "Plugin for Nix that allows for using LUA as a programming language";
		homepage = "https://github.com/KubC08/nix-lua";
		license = licenses.gpl3;
		maintainers = with maintainers; [ kubc08 ];
		platforms = [ "x86_64-linux" ];
	};
}
