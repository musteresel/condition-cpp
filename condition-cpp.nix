{stdenv, cmake, doxygen}:

stdenv.mkDerivation {
  name = "condition-cpp";
  src = ./.;
  nativeBuildInputs = [cmake doxygen];
}
