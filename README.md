# chess-torunament

System zarządzania turniejami szachowymi.

Projekt na zajęcia "Wprowadzenie do pracy projektowej metodą PBL"

## Kompilacja

### Linux

Wymagany CMake oraz kompilator (GCC/Clang)

1. `cmake -B build .`
2. `cmake --build build`

Opcjonalnie krok 1. można wykonać z `CXX=clang++` i/lub `-G Ninja`:

`CXX=clang++ cmake -B build -G Ninja .`

### Windows

Wymagany Visual Studio

1. Otwórz CMakeLists.txt w Visual Studio jako projekt.
2. Ctrl+Shift+B aby skompilować
3. Ctrl+F5 aby uruchomić

## Autorzy

Michał Tomczyk, Łukasz Tomczyk
