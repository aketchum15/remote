{
    description = "build env for remote";
    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05"; # pinned to version 24.05
        flakeutils.url = "github:numtide/flake-utils"
    };

    outputs = {self, nixpkgs, flake-utils }: 
        flake-utils.lib.eachDefaultSystem (system:
            let
                pkgs = import nixpkgs {
                    inherit system;
                };
            in {
                devShells.default = pkgs.mkShell {
                    name = "dev-shell-remote";

                    nativeBuildInputs = with pkgs; [
                        pkg-config
                        cmake
                        gcc
                    ];

                    buildInputs = with pkgs; [
                        alsa-lib
                        libgpiod
                        libopus
                        nanopb
                    ];

                    shellHook = "echo shell init complete";
                };
            }
        );

}
