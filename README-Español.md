# BtScreenLocker
Este demonio (del inglés daemon) busca dispositivos Bluetooth de confianza para saber si están cerca, y si no lo están, bloquea tu pantalla para evitar acceso no autorizado a tu computadora.

# Compilando e Instalando
Primero que nada, asegúrate de tener todas las dependencias instaladas en tu sistema, estas son:
| Dependencia | Módulo                             |
|-------------|------------------------------------|
| Qt6         | Bluetooth, DBus, Linguist, Widgets |

## Instalando las Dependencias
Esta es una tabla no exhaustiva de cómo puedes instalar las dependiencias, dependiendo de tu distribución Linux:

**Asegúrate de correr el comando correspondiente como super usuario.**
| Distribución              | Comando                                                              |
|---------------------------|----------------------------------------------------------------------|
| Arch Linux                | pacman -S qt6-base qt6-connectivity qt6-tools                        |
| Debian, Ubuntu and alike  | apt install libqt6bluetooth6 libqt6dbus6 linguist-qt6 libqt6widgets6 |
| Fedora                    | dnf install qt6-qtbase-devel qt6-qtconnectivity-devel qt6-linguist   |
| Gentoo                    | emerge qtbase (+dbus +widgets) qtbluetooth                           |

**Nota:** No todas las distribuciones Linux ofrecen Qt6, en caso de que sólo tengas Qt5 disponible, edita `CMakeLists.txt` y busca esta línea por el principio: `find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Bluetooth Core LinguistTools Widgets DBus)` y elimina Qt6 de ahí.

Entonces debería quedar como: `find_package(QT NAMES Qt5 REQUIRED COMPONENTS Bluetooth Core LinguistTools Widgets DBus)`.

Aunque desarrollo este software en Qt6, ¡Trato de hacerlo compatible con Qt5!
## Obteniendo las Fuentes del Proyecto
Ya que tienes todas las dependiencias instaladas en tu sistema, obtén las fuentes del proyecto con:
```
git clone https://github.com/brookiestein/BtScreenLocker
```
### Compilando
Este proyecto está hecho usando CMake como sistema de construcción, haciendo este proceso tan sencillo como:
```
mkdir BtScreenLocker/build
cd BtScreenLocker
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release
sudo cmake --build build --target install --parallel `nproc`
```

Lo que hiciste fue:
1. Creaste una carpeta `build` para separar los archivos de compilación de las fuentes (no obligatorio).
2. Entraste a la carpeta del proyecto.
3. Le indicaste a CMake cómo compilar e instalar el programa, y;
4. Compilaste e instalar el programa con el mismo comando.
5. --parallel `nproc` es para que CMake use todos los hilos de tu procesador y la compilación sea más rápida.

CMake te dirá dónde instaló BtScreenLocker, pero normalmente se instala en `/usr/local`.

# Cómo usar BtScreenLocker
Usar BtScreenLocker es bastante sencillo, en realidad. La primera vez que lo corras, escaneará dispositivos Bluetooth cercanos y después de unos segundos, te mostrará los que encontró. **¡Por favor asegúrate de que tu dispositivo de confianza sea reconocible!**

En la ventana que apareció, haz doble click en el dispositivo en el que quieres confiar y BtScreenLocker lo guardará en la lista de confianza.

Después de eso, BtScreenLocker correrá como un demonio (del inglés daemon) sin necesidad de interacción contigo. Cada 30 segundos, escaneará de nuevo buscando ese dispositivo de confianza y, si determina que ya no está cerca, bloqueará tu pantalla para proteger tu computadora de acceso no autorizado.

Tan pronto como desbloquees tu computadora, empezará a escanear de nuevo cada 30 segundos.

Si, por alguna razón, quieres detener el escaneo y, en consecuencia, el bloqueo de tu pantalla si ya no hay dispositivos de confianza cerca, corre en una terminal:
```
BtScreenLocker -p
```
`-p` es la versión corta de `--pause`.

Eso le dirá al BtScreenLocker, que ya está corriendo, que detenga el escaneo sin cerrarse.

Si en algún momento quieres resumir el escaneo, haz el proceso opuesto, corre un una terminal:
```
BtScreenLocker -r
```
`-r` es la versión corta de `--resume`.

¡Las cosas se ponen incluso más interesantes! Después de la primera ejecución, BtScreenLocker no escanerá dispositivos Bluetooth de nuevo con el objetivo de agregar más a la lista de confianza porque ya hay uno, pero puedes agregar más en cualquier momento corriendo:
```
BtScreenLocker -d
```
`-d` es la versión corta de `--discover`.

¡Incluso más interesante! Si ya tienes una instancia de BtScreenLocker corriendo, sólo ejecuta:
```
BtScreenLocker -s
```
`-s` es la versión corta de `--scan-again`.

Eso le dirá a la instancia de BtScreenLocker, que ya está corriendo, que detenga el escaneo de dispositivos de confianza y que en su lugar escanee de nuevo, ¡pero con el objetivo de agregar más a la lista de confianza!

Tanto si agregaste un nuevo dispositivo Bluetooth de confianza como si no, BtScreenLocker empezará el escaneo en busca de los dispositivos Bluetooth de confianza que ya tenías (incluyendo el que posiblemente agregaste).

# Lista de Opciones
| Opción Larga | Opción Corta  | Descripción                                                                                 |
|--------------|---------------|---------------------------------------------------------------------------------------------|
| --autostart  | -a            | Registra BtScreenLocker para que se inicie automáticamente cuando tu PC arranca. (con args) |
| --debug      | -D            | Activa el registro para debuggear (activa el modo verboso de manera implícita).             |
| --discover   | -d            | Escanea dispositivos Bluetooth para agregarlos a la lista de confianza.                     |
| --filename   | -f            | Donde guardar los registros (siempre verbose, depurar depende de --debug).                  |
| --help       | -h            | Muestra esta ayuda.                                                                         |
| --kill       | -k            | Termina una instancia de BtScreenLocker que ya esté corriendo.                              |
| --language   | -l            | Establecer idioma. (Disponibles: Inglés (por defecto), y Español).                          |
| --pause      | -p            | Pausa una instancia de BtScreenLocker que ya esté corriendo.                                |
| --resume     | -r            | Resume una instancia de BtScreenLocker que ya esté corriendo.                               |
| --scan-again | -s            | Igual que --discover, pero para una instancia de BtScreenLocker que ya esté corriendo.      |
| --verbose    | -V            | Activa el registro en modo verboso.                                                         |
| --version    | -v            | Muestra la versión de este programa.                                                        |
