Pour reproduire le fonctionnement de `j.get<Person>()` avec une fonction `get<Person>()` en utilisant ADL (Argument-Dependent Lookup), nous allons créer une fonction template `get` qui utilisera la fonction `from_json` définie dans le même espace de noms que la structure `Person`. 

### Étape 1: Définir la structure `Person` et la fonction `from_json`

Nous définissons la structure `Person` et la fonction `from_json` dans le même espace de noms. La fonction `from_json` extrait les valeurs JSON et les affecte à une instance de `Person`.

```cpp
#include <iostream>
#include <string>

struct Person {
    double pi;
    bool happy;
    std::string name;
    int age;
};

// Fonction de conversion from_json dans le même espace de noms que Person
void from_json(const std::string& json_str, Person& p) {
    // Simule une conversion à partir d'une chaîne JSON (pour cet exemple, assignation statique)
    p.pi = 3.141;
    p.happy = true;
    p.name = "John Doe";
    p.age = 30;
}
```

### Étape 2: Définir la fonction template `get`

Nous définissons une fonction template `get` qui appelle la fonction `from_json` appropriée pour le type demandé. Cette fonction `get` utilise la fonction `from_json` trouvée grâce à ADL.

```cpp
template<typename T>
T get(const std::string& json_str) {
    T obj;
    from_json(json_str, obj); // ADL trouve la fonction from_json appropriée
    return obj;
}
```

### Étape 3: Utiliser la fonction `get` dans le `main`

Nous utilisons la fonction `get` pour convertir une chaîne JSON en une instance de `Person`.

```cpp
int main() {
    // Simule une chaîne JSON d'exemple (non utilisée dans la conversion simplifiée)
    std::string json_str = R"({"pi": 3.141, "happy": true, "name": "John Doe", "age": 30})";

    // Convertit le JSON en une instance de Person en utilisant la fonction get
    Person person = get<Person>(json_str);

    // Affiche les valeurs
    std::cout << "pi: " << person.pi << std::endl;
    std::cout << "happy: " << std::boolalpha << person.happy << std::endl;
    std::cout << "name: " << person.name << std::endl;
    std::cout << "age: " << person.age << std::endl;

    return 0;
}
```

### Code complet

Voici le code complet de l'exemple avec la fonction `get` :

```cpp
#include <iostream>
#include <string>

struct Person {
    double pi;
    bool happy;
    std::string name;
    int age;
};

// Fonction de conversion from_json dans le même espace de noms que Person
void from_json(const std::string& json_str, Person& p) {
    // Simule une conversion à partir d'une chaîne JSON (pour cet exemple, assignation statique)
    p.pi = 3.141;
    p.happy = true;
    p.name = "John Doe";
    p.age = 30;
}

// Fonction template get qui utilise ADL pour appeler from_json
template<typename T>
T get(const std::string& json_str) {
    T obj;
    from_json(json_str, obj); // ADL trouve la fonction from_json appropriée
    return obj;
}

int main() {
    // Simule une chaîne JSON d'exemple (non utilisée dans la conversion simplifiée)
    std::string json_str = R"({"pi": 3.141, "happy": true, "name": "John Doe", "age": 30})";

    // Convertit le JSON en une instance de Person en utilisant la fonction get
    Person person = get<Person>(json_str);

    // Affiche les valeurs
    std::cout << "pi: " << person.pi << std::endl;
    std::cout << "happy: " << std::boolalpha << person.happy << std::endl;
    std::cout << "name: " << person.name << std::endl;
    std::cout << "age: " << person.age << std::endl;

    return 0;
}
```

### Compilation et exécution

Pour compiler et exécuter ce programme :

```sh
g++ -std=c++17 -o get_example main.cpp
./get_example
```

Ce programme affichera les valeurs de l'instance de `Person` initialisées par la fonction `from_json`. La fonction template `get` utilise ADL pour trouver et appeler la fonction `from_json` appropriée, reproduisant ainsi le comportement de `j.get<Person>()` de la bibliothèque JSON de nlohmann.