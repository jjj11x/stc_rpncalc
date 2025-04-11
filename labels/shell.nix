{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell rec {
	buildInputs = with pkgs; [
		python312
		python312Packages.drawsvg # https://pypi.org/project/drawsvg/
	];
}
