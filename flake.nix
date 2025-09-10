{
    description = "";
    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";
        flake-utils.url = "github:numtide/flake-utils";
    };

    outputs = {self, nixpkgs, flake-utils }:
        flake-utils.lib.eachDefaultSystem (hostSystem:
            let 
                targetSystem = "aarch64-linux";

                pkgsHost = import nixpkgs {
                    system = hostSystem;
                };
                pkgsCross = import nixpkgs {
                    system = hostSystem;
                    crossSystem = {
                        config = targetSystem;
                    };
                };
                voice = pkgsCross.stdenv.mkDerivation {
                    pname = "voice";
                    version = "0.1.0";

                    src = ./voice;

                    nativeBuildInputs = with pkgsHost; [
                        pkg-config
                        cmake
                    ];
                    buildInputs = with pkgsCross; [
                        alsa-lib
                        libgpiod
                        libopus
                    ];

                    cmakeFlags = [
                        "-DCMAKE_BUILD_TYPE=Debug"
                    ];

                    makeFlags = [
                        "VERBOSE=1"
                    ];

                };
            in {
                nixosConfigurations = {
                    zero2w = nixpkgs.lib.nixosSystem {
                        system = targetSystem;
                        modules = [
                            "${nixpkgs}/nixos/modules/installer/sd-card/sd-image-aarch64.nix"
                            ./nixos-pi-zero-2/zero2w.nix
                            {
                                environment.systemPackages = with pkgsCross; [
                                    voice
                                    libopus
                                    libgpiod 
                                    alsa-lib
                                ];
                                nixpkgs.buildPlatform = hostSystem;
                                nixpkgs.hostPlatform = targetSystem;
                            }
                        ];
                    };
                };
                packages.voice = voice;

                devShells.voice = pkgsHost.mkShell {
                  packages = with pkgsHost; [
                    cmake
                    pkg-config
                    clang-tools
                    clang
                    alsa-lib
                    libgpiod
                    libopus
                    nanopb
                  ];

                  shellHook = ''
                    alias v=nvim
                    export CC=clang
                    export CXX=clang++

                    cd voice
                    mkdir -p build
                    cd build

                    # Only configure — no building needed
                    cmake .. \
                      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                      -DCMAKE_BUILD_TYPE=Debug \
                      -DCMAKE_C_COMPILER=clang \
                      -DCMAKE_CXX_COMPILER=clang++ \
                      -G Ninja

                    # symlink for clangd (must be in project root)
                    ln -sf build/compile_commands.json ../compile_commands.json
                    cd ..
                  '';
                };
            }
        );
}
