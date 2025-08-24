{
    description = "Cross compile remote for aarch64"; # inputs that are needed to generate the outputs 
    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11"; # pinned to version 24.11
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
            in {
                packages.default = pkgsCross.stdenv.mkDerivation {
                    pname = "remote";
                    version = "0.1.0";

                    src = ./.;


                    nativeBuildInputs = with pkgsHost; [
                        pkg-config
                        cmake
                    ];
                    buildInputs = with pkgsCross; [
                        alsa-lib
                        libgpiod
                        libopus
                        nanopb
                    ];

                    cmakeFlags = [
                        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
                        "-DCMAKE_BUILD_TYPE=Debug"
                    ];

                    postInstall = ''
                        cp compile_commands.json $out/
                    '';
                };

                devShells.default = pkgsHost.mkShell {
                  packages = with pkgsHost; [
                    cmake
                    pkg-config
                    clang-tools   # For clangd (includes tools like clangd-indexer, clang-tidy)
                    clang         # Host compiler (required for stdlib headers)
                    alsa-lib
                    libgpiod
                    libopus
                    nanopb
                  ];

                  shellHook = ''
                    export CC=clang
                    export CXX=clang++

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
