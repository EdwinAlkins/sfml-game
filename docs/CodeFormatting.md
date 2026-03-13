# Code Formatting and Analysis

Ce document explique comment utiliser `clang-format` et `clang-tidy` pour maintenir la qualité du code C++ dans ce projet.

## Table des matières

- [Installation](#installation)
- [clang-format](#clang-format)
- [clang-tidy](#clang-tidy)
- [Configuration](#configuration)
- [Intégration CMake](#intégration-cmake)
- [Scripts d'automatisation](#scripts-dautomatisation)
- [Exemples pratiques](#exemples-pratiques)

## Installation

### Ubuntu/Debian
```bash
# Installation des outils
sudo apt install clang-format clang-tidy

# Ou version spécifique
sudo apt install clang-format-15 clang-tidy-15
```

### Vérification
```bash
clang-format --version
clang-tidy --version
```

## clang-format

### Utilisation basique

**Formatage d'un fichier :**
```bash
clang-format -i mon_fichier.cpp  # -i pour modifier directement
clang-format mon_fichier.cpp     # affiche sans modifier
```

**Formatage multiple :**
```bash
# Tous les fichiers C++ du projet
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# Fichiers spécifiques
clang-format -i core/*.cpp core/*.hpp main.cpp
```

### Styles prédéfinis
```bash
clang-format -style=Google -i mon_fichier.cpp
clang-format -style=LLVM -i mon_fichier.cpp
clang-format -style=Chromium -i mon_fichier.cpp
```

## clang-tidy

### Utilisation basique

**Analyse d'un fichier :**
```bash
clang-tidy mon_fichier.cpp -- -std=c++17 -I./core
```

**Analyse avec corrections :**
```bash
clang-tidy mon_fichier.cpp --fix -- -std=c++17 -I./core
```

**Analyse multiple :**
```bash
find . -name "*.cpp" | xargs clang-tidy -- -std=c++17 -I./core
```

### Checks recommandés

```bash
# Checks de base
clang-tidy mon_fichier.cpp -- -checks=readability-*,performance-* -std=c++17

# Checks modernes
clang-tidy mon_fichier.cpp -- -checks=modernize-*,cppcoreguidelines-* -std=c++17

# Tous les checks (peut être verbeux)
clang-tidy mon_fichier.cpp -- -checks=* -std=c++17
```

## Configuration

### .clang-format

Créez un fichier `.clang-format` à la racine du projet :

```yaml
# Style de base
BasedOnStyle: Google

# Indentation
IndentWidth: 4
TabWidth: 4
UseTab: Never

# Espacement
SpaceAfterCStyleCast: true
SpaceBeforeParens: ControlStatements
SpaceInEmptyParentheses: false

# Longueur de ligne
ColumnLimit: 100

# Accolades
BreakBeforeBraces: Attach
Cpp11BracedListStyle: true

# Includes
SortIncludes: true
IncludeBlocks: Regroup

# Spécifique C++
NamespaceIndentation: None
AccessModifierOffset: -2
```

### .clang-tidy

Créez un fichier `.clang-tidy` à la racine du projet :

```yaml
Checks: >
  *,
  -abseil-*,
  -altera-*,
  -android-*,
  -fuchsia-*,
  -google-*,
  -llvm-*,
  -llvmlibc-*,
  -zircon-*

WarningsAsErrors: ''
HeaderFilterRegex: '.*'
AnalyzeTemporaryDtors: false
FormatStyle: 'file'

CheckOptions:
  - key: readability-identifier-naming.ClassCase
    value: CamelCase
  - key: readability-identifier-naming.FunctionCase
    value: camelBack
  - key: readability-identifier-naming.VariableCase
    value: camelBack
  - key: readability-identifier-naming.ParameterCase
    value: camelBack
  - key: readability-identifier-naming.MemberCase
    value: camelBack
```

## Intégration CMake

Ajoutez dans votre `CMakeLists.txt` principal :

```cmake
# clang-format
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting code with clang-format"
    )
endif()

# clang-tidy
find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY};-checks=*;-warnings-as-errors=*"
    )
endif()
```

## Scripts d'automatisation

### format_and_check.sh

Créez un script `scripts/format_and_check.sh` :

```bash
#!/bin/bash

set -e

echo "🔧 Formatting code with clang-format..."
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

echo "🔍 Running clang-tidy analysis..."
find . -name "*.cpp" | while read file; do
    echo "Checking $file"
    clang-tidy "$file" -- -std=c++17 -I./core -I./build/_deps/sfml-src/include
done

echo "✅ Code formatting and analysis complete!"
```

### pre-commit hook

Créez `.git/hooks/pre-commit` :

```bash
#!/bin/bash

echo "Running pre-commit checks..."

# Formatage
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# Vérification des changements
if ! git diff --quiet; then
    echo "Code was reformatted. Please commit again."
    exit 1
fi

echo "Pre-commit checks passed!"
```

## Exemples pratiques

### Pour ce projet SFML

**Formatage complet :**
```bash
clang-format -i core/*.cpp core/*.hpp core/*/*.cpp core/*/*.hpp main.cpp
```

**Analyse d'un fichier spécifique :**
```bash
clang-tidy core/GameWorld.cpp -- -std=c++17 -I./core -I./build/_deps/sfml-src/include -I./build/_deps/fmt-src/include -I./build/_deps/spdlog-src/include -I./build/_deps/box2d-src/include -I./build/_deps/json-src/include -I./build/_deps/tgui-src/include -I./build/_deps/eigen-src -I./build/_deps/cppfs-src/include
```

**Analyse avec corrections :**
```bash
clang-tidy core/GameWorld.cpp --fix -- -std=c++17 -I./core -I./build/_deps/sfml-src/include
```

### Commandes utiles

```bash
# Voir les différences avant formatage
clang-format core/GameWorld.cpp | diff core/GameWorld.cpp -

# Formatage en mode dry-run
clang-format core/GameWorld.cpp

# Analyse avec sortie dans un fichier
clang-tidy core/GameWorld.cpp -- -std=c++17 -I./core > analysis.txt 2>&1

# Formatage avec style spécifique
clang-format -style="{BasedOnStyle: Google, IndentWidth: 2}" -i core/GameWorld.cpp
```

## Intégration IDE

### VS Code
- Extension "C/C++" (Microsoft)
- Extension "clang-format"
- Configuration dans `.vscode/settings.json` :
```json
{
    "C_Cpp.clang_format_style": "file",
    "editor.formatOnSave": true
}
```

### CLion
- Support natif intégré
- Activation dans Settings > Editor > Code Style > C/C++

### Vim/Neovim
- Plugin `vim-clang-format`
- Mapping : `nnoremap <leader>f :ClangFormat<CR>`

## Bonnes pratiques

1. **Formatage automatique** : Configurez votre IDE pour formater à la sauvegarde
2. **Hooks Git** : Utilisez des pre-commit hooks pour vérifier le formatage
3. **CI/CD** : Intégrez les vérifications dans votre pipeline
4. **Configuration partagée** : Commitez les fichiers `.clang-format` et `.clang-tidy`
5. **Formation équipe** : Assurez-vous que tous les développeurs utilisent les mêmes outils

## Résolution de problèmes

### Erreurs communes

**"clang-tidy: command not found"**
```bash
sudo apt install clang-tidy
```

**"No such file or directory" avec clang-tidy**
- Vérifiez les chemins d'inclusion avec `-I`
- Assurez-vous que les dépendances sont compilées

**Formatage incohérent**
- Vérifiez la présence du fichier `.clang-format`
- Utilisez `clang-format -style=file` pour forcer l'utilisation du fichier de config

### Debug

```bash
# Voir la configuration utilisée
clang-format -dump-config

# Vérifier les includes
clang-tidy -list-checks

# Mode verbose
clang-tidy -v mon_fichier.cpp -- -std=c++17 -I./core
```

## Ressources

- [Documentation officielle clang-format](https://clang.llvm.org/docs/ClangFormat.html)
- [Documentation officielle clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
- [Liste des checks clang-tidy](https://clang.llvm.org/extra/clang-tidy/checks/list.html)
- [Styles prédéfinis clang-format](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
