let
    generateTestsForPlatform = { pkgs, path, phpAttr }:
        pkgs.recurseIntoAttrs {
            gcc = let
                php = pkgs.${phpAttr};
            in pkgs.callPackage ../default.nix {
                inherit php;
                buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
            };
            clang = let
                php = pkgs.${phpAttr};
                stdenv = pkgs.clangStdenv;
            in pkgs.callPackage ../default.nix {
                inherit php stdenv;
                buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php stdenv; };
            };
            # not working due to debuginfo issue
            # "error adding symbols: file in wrong format"
            # probably due to cross compiling
            # gcc-i686 = let
            #     php = pkgs.pkgsi686Linux.${phpAttr};
            # in pkgs.pkgsi686Linux.callPackage ../default.nix {
            #     inherit php;
            #     buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
            # };
        };
in
builtins.mapAttrs (k: _v:
  let
    path = builtins.fetchTarball {
        url = https://github.com/NixOS/nixpkgs-channels/archive/nixos-20.03.tar.gz;
        name = "nixpkgs-20.03";
    };
    pkgs = import (path) { system = k; };
  in
  pkgs.recurseIntoAttrs {
    php72 = generateTestsForPlatform {
        inherit pkgs path;
        phpAttr = "php72";
    };

    php73 = generateTestsForPlatform {
        inherit pkgs path;
        phpAttr = "php73";
    };

    php74 = generateTestsForPlatform {
        inherit pkgs path;
        phpAttr = "php74";
    };
  }
) {
  x86_64-linux = {};
  # Uncomment to test build on macOS too
  # x86_64-darwin = {};
}
