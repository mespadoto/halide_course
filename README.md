# halide_course

## Pré-requisitos
Antes de começar, certifique-se de ter o Docker instalado no seu computador.

## Comandos

### 1. `docker run -it -v .:/src local:halidecpp`
Este comando inicia um contêiner Docker com a imagem local chamada `halidecpp`, que contém todas as dependências necessárias para executar os exercícios HalideCpp. A opção `-v .:/src` mapeia o diretório atual para `/src` no contêiner, permitindo que você acesse e modifique os arquivos durante o desenvolvimento.

### 2. `cd src/<exercise_directory>`
Mude para o diretório do exercício desejado. Substitua `<exercise_directory>` pelo nome do diretório específico do exercício que você deseja explorar.

### 3. `cmake .`
Este comando configura o ambiente de compilação usando o CMake. Ele lê o arquivo `CMakeLists.txt` no diretório do exercício e prepara a compilação do código-fonte HalideCpp.

### 4. `make`
Execute este comando para compilar o código-fonte do exercício. O makefile gerado pelo CMake compilará os arquivos necessários e gerará um executável chamado `main`.

### 5. `./main`
Finalmente, execute o programa principal gerado após a compilação. Este comando executa o programa HalideCpp resultante, que contém a implementação do exercício. Dependendo do exercício, pode haver argumentos adicionais necessários para a execução.
