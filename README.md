# Hot-Shot-Arena

Para instalar as dependências (siga de acordo com seu sistema operacional):

### Linux (Debian/Ubuntu)

```sh
sudo apt-get update
sudo apt-get install build-essential pkg-config libraylib-dev libgl1-mesa-dev
```

### Linux (Arch)

```sh
sudo pacman -Sy base-devel raylib
```

### Linux (Fedora)

```sh
sudo dnf install @development-tools raylib-devel
```

### macOS

Instale o [Homebrew](https://brew.sh/) e depois:

```sh
brew install raylib
```

### Windows

Recomenda-se usar [MSYS2](https://www.msys2.org/):

```sh
pacman -Syu mingw-w64-x86_64-raylib
```

## Compilação

Para compilar o jogo, execute:

```sh
make
```

## Executando o Jogo

Após compilar, rode:

```sh
make run
```

## Limpeza dos arquivos de build

```sh
make clean

Divirta-se!

Grupo: Clarissa Oliveira Lopes, Denys Sales, Pedro Abner, Rafael Padilha