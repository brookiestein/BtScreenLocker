<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es_US">
<context>
    <name>BluetoothListener</name>
    <message>
        <source>There isn&apos;t a valid Bluetooth device on this machine. Can&apos;t do anything.</source>
        <translation type="vanished">No hay un dispositivo Bluetooth válido en esta computadora. No se puede hacer nada.</translation>
    </message>
    <message>
        <source>Error</source>
        <translation type="vanished">Error</translation>
    </message>
    <message>
        <source>Discoverying devices...</source>
        <translation type="vanished">Escaneando dispositivos...</translation>
    </message>
    <message>
        <source>Discoverying devices</source>
        <translation type="vanished">Escaneando dispositivos</translation>
    </message>
    <message>
        <source>Please wait up to 30 seconds, I&apos;ll show you the devices I discover.</source>
        <translation type="vanished">Por favor, espera 30 segundos. Te mostraré los dispositivos que encuentre.</translation>
    </message>
    <message>
        <source>Bluetooth device: %1, %2 became unavailable. Can&apos;t do anything.</source>
        <translation type="vanished">Dispositivo Bluetooth: %1, %2 ya no está disponible. No se puede hacer nada.</translation>
    </message>
    <message>
        <source>Bluetooth device: %1, %2 became unavailable.
Ending program execution because of this!

If at some point the device: %3 becomes available again, please re-run me.</source>
        <translation type="vanished">El dispositivo Bluetooth: %1, %2 ya no está disponible.\n
¡Terminando ejecución del programa!\n\n
Si en algún momento el dispositivo: %3 vuelve a estar disponible, por favor ejecútame de nuevo.</translation>
    </message>
    <message>
        <source>Device discovery finished.</source>
        <translation type="vanished">Escaneo de dispositivos finalizado.</translation>
    </message>
    <message>
        <source>Adding device %1 to the trusted devices list...</source>
        <translation type="vanished">Agregando el dispositivo: %1 a la lista de dispositivos de confianza...</translation>
    </message>
    <message>
        <source>Warning</source>
        <translation type="vanished">Advertencia</translation>
    </message>
    <message>
        <source>You haven&apos;t added any device to the trusted list.
Would you like to scan for devices again?</source>
        <translation type="vanished">No has agregado ningún dispositivo a la lista de confianza.\n
¿Te gustaría escanear los dispositivos de nuevo?</translation>
    </message>
    <message>
        <source>Information</source>
        <translation type="vanished">Información</translation>
    </message>
    <message>
        <source>No looking for trusted devices because there isn&apos;t any.
You can scan for devices again at any time by calling another instance of this program with --scan-again without closing this one.</source>
        <translation type="vanished">No se buscarán dispositivos de confianza porque no hay.\n
Puedes escanear los dispositivos de nuevo en cualquier momento ejecutando otra instancia de este programa con --scan-again sin cerrar este.</translation>
    </message>
    <message>
        <source>Locking screen because all trusted devices are far way.</source>
        <translation type="vanished">Bloqueando la pantalla porque todos los dispositivos de confianza están lejos.</translation>
    </message>
    <message>
        <source>Not locking screen because the following trusted devices are near:
%1</source>
        <translation type="vanished">No se bloqueará la pantalla porque los siguientes dispositivos de confianza están cerca:\n%1</translation>
    </message>
    <message>
        <source>Quitting because of D-Bus signal telling me so...</source>
        <translation type="vanished">Saliendo por señal D-Bus pidiéndomelo...</translation>
    </message>
    <message>
        <source>Not locking screen because the following trusted devices are near: %1</source>
        <translation type="vanished">No se bloqueará la pantalla porque los siguientes dispositivos de confianza están cerca: %1</translation>
    </message>
    <message>
        <source>Checking whether trusted devices are near...</source>
        <translation type="vanished">Verificando si hay dispositivos de confianza cerca...</translation>
    </message>
    <message>
        <source>Looking for trusted devices is already stopped, ignoring message.</source>
        <translation type="vanished">Buscar dispositivos de confianza ya está detenido, ignorando mensaje.</translation>
    </message>
    <message>
        <source>Looking for trusted devices stopped by D-Bus signal telling me so.</source>
        <translation type="vanished">Buscar dispositivos de confianza detenido por orden de la señal D-Bus recibida.</translation>
    </message>
    <message>
        <source>Looking for trusted devices isn&apos;t stopped, ignoring message.</source>
        <translation type="vanished">Buscar dispositivos de confianza no está detenido, ignorando mensaje.</translation>
    </message>
    <message>
        <source>Looking for trusted devices again.</source>
        <translation type="vanished">Buscando dispositivos de confianza de nuevo.</translation>
    </message>
    <message>
        <source>Discoverying device started by D-Bus signal telling me so.</source>
        <translation type="vanished">Escanear dispositivos iniciado por la señal D-Bus recibida.</translation>
    </message>
</context>
<context>
    <name>Connection</name>
    <message>
        <location filename="../src/connection.cpp" line="42"/>
        <source>Trying to connect to Bluetooth device: %1 - %2...</source>
        <translation>Intentando conectarse al dispositivo Bluetooth: %1 - %2...</translation>
    </message>
    <message>
        <location filename="../src/connection.cpp" line="48"/>
        <source>Couldn&apos;t allocate socket to connect to Bluetooth device: %1
Error: %2</source>
        <translation>No se pudo reservar memoria para el socket que conectará con el dispositivo Bluetooth: %1\n
Error: %2</translation>
    </message>
    <message>
        <location filename="../src/connection.cpp" line="59"/>
        <source>Connected to Bluetooth device: %1 - %2!</source>
        <translation>Conexión establecida con el dispositivo Bluetooth: %1 - %2!</translation>
    </message>
    <message>
        <location filename="../src/connection.cpp" line="65"/>
        <source>Couldn&apos;t connect to Bluetooth device: %1 - %2.
It may be far or has Bluetooth disabled.</source>
        <translation>No se pudo conectar con el dispositivo Bluetooth: %1 - %2.\n
Puede estar lejos o tener el Bluetooth desactivado.</translation>
    </message>
</context>
<context>
    <name>DeviceChooser</name>
    <message>
        <source>Form</source>
        <translation type="vanished">Dispositivos Encontrados</translation>
    </message>
    <message>
        <location filename="../src/devicechooser.ui" line="26"/>
        <location filename="../build/Desktop-Debug/BtScreenLocker_autogen/include/ui_devicechooser.h" line="86"/>
        <location filename="../build/release/BtScreenLocker_autogen/include/ui_devicechooser.h" line="86"/>
        <source>Choose your Device</source>
        <translation>Selecciona tu Dispositivo</translation>
    </message>
    <message>
        <location filename="../src/devicechooser.ui" line="51"/>
        <location filename="../build/Desktop-Debug/BtScreenLocker_autogen/include/ui_devicechooser.h" line="87"/>
        <location filename="../build/release/BtScreenLocker_autogen/include/ui_devicechooser.h" line="87"/>
        <source>Add</source>
        <translation>Añadir</translation>
    </message>
    <message>
        <location filename="../src/devicechooser.ui" line="14"/>
        <location filename="../build/Desktop-Debug/BtScreenLocker_autogen/include/ui_devicechooser.h" line="85"/>
        <location filename="../build/release/BtScreenLocker_autogen/include/ui_devicechooser.h" line="85"/>
        <location filename="../src/devicechooser.cpp" line="9"/>
        <source>Discovered Devices</source>
        <translation>Dispositivos Encontrados</translation>
    </message>
    <message>
        <location filename="../src/devicechooser.cpp" line="37"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../src/devicechooser.cpp" line="37"/>
        <source>MAC Address</source>
        <translation>MAC Address</translation>
    </message>
    <message>
        <location filename="../src/devicechooser.cpp" line="37"/>
        <source>Signal strength</source>
        <translation>Fuerza de la Señal</translation>
    </message>
</context>
<context>
    <name>Listener</name>
    <message>
        <location filename="../src/listener.cpp" line="24"/>
        <source>There isn&apos;t a valid Bluetooth device on this machine. Can&apos;t do anything.</source>
        <translation>No hay un dispositivo Bluetooth válido en esta computadora. No se puede hacer nada.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="26"/>
        <location filename="../src/listener.cpp" line="99"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="81"/>
        <source>Discoverying devices...</source>
        <translation>Escaneando dispositivos...</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="86"/>
        <source>Discoverying devices</source>
        <translation>Escaneando dispositivos</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="87"/>
        <source>Please wait up to 30 seconds, I&apos;ll show you the devices I discover.</source>
        <translation>Por favor, espera 30 segundos. Te mostraré los dispositivos que encuentre.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="95"/>
        <source>Bluetooth device: %1, %2 became unavailable.
Ending program execution because of this!

If at some point the device: %3 becomes available again, please re-run me.</source>
        <translation>El dispositivo Bluetooth: %1, %2 ya no está disponible.\n
¡Terminando ejecución del programa!\n\n
Si en algún momento el dispositivo: %3 vuelve a estar disponible, por favor ejecútame de nuevo.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="107"/>
        <source>Device discovery finished.</source>
        <translation>Escaneo de dispositivos finalizado.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="128"/>
        <source>Adding device %1 to the trusted devices list...</source>
        <translation>Agregando el dispositivo: %1 a la lista de dispositivos de confianza...</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="136"/>
        <source>Warning</source>
        <translation>Advertencia</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="137"/>
        <source>You haven&apos;t added any device to the trusted list.
Would you like to scan for devices again?</source>
        <translation>No has agregado ningún dispositivo a la lista de confianza.\n
¿Te gustaría escanear los dispositivos de nuevo?</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="145"/>
        <source>Information</source>
        <translation>Información</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="146"/>
        <source>No looking for trusted devices because there isn&apos;t any.
You can scan for devices again at any time by calling another instance of this program with --scan-again without closing this one.</source>
        <translation>No se buscarán dispositivos de confianza porque no hay.\n
Puedes escanear los dispositivos de nuevo en cualquier momento ejecutando otra instancia de este programa con --scan-again sin cerrar este.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="171"/>
        <source>Locking screen because all trusted devices are far way.</source>
        <translation>Bloqueando la pantalla porque todos los dispositivos de confianza están lejos.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="189"/>
        <source>Not locking screen because the following trusted devices are near:
%1</source>
        <translation>No se bloqueará la pantalla porque los siguientes dispositivos de confianza están cerca:\n%1</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="160"/>
        <source>Checking whether trusted devices are near...</source>
        <translation>Verificando si hay dispositivos de confianza cerca...</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="207"/>
        <source>Screen active. Looking for trusted devices again.</source>
        <translation>Pantalla activa. Buscando dispositivos de confianza de nuevo.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="204"/>
        <source>Not looking for trusted devices because screen is locked.</source>
        <translation>No se buscarán dispositivos de confianza porque la pantalla está bloqueada.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="217"/>
        <source>Looking for trusted devices is already stopped, ignoring message.</source>
        <translation>Buscar dispositivos de confianza ya está detenido, ignorando mensaje.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="221"/>
        <source>Looking for trusted devices stopped by D-Bus signal telling me so.</source>
        <translation>Buscar dispositivos de confianza detenido por orden de la señal D-Bus recibida.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="231"/>
        <source>Looking for trusted devices isn&apos;t stopped, ignoring message.</source>
        <translation>Buscar dispositivos de confianza no está detenido, ignorando mensaje.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="235"/>
        <source>Looking for trusted devices again.</source>
        <translation>Buscando dispositivos de confianza de nuevo.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="242"/>
        <source>Discoverying device started by D-Bus signal telling me so.</source>
        <translation>Escanear dispositivos iniciado por la señal D-Bus recibida.</translation>
    </message>
    <message>
        <location filename="../src/listener.cpp" line="248"/>
        <source>Quitting because of D-Bus signal telling me so...</source>
        <translation>Saliendo por señal D-Bus pidiéndomelo...</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>Using language: %1</source>
        <translation type="vanished">Usando idioma: %1</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="50"/>
        <source>Sending kill signal...</source>
        <translation>Enviando señal para terminar...</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="57"/>
        <source>Sending pause signal...</source>
        <translation>Enviando señal para pausar...</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="64"/>
        <source>Sending resume signal...</source>
        <translation>Enviando señal para resumir...</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="71"/>
        <source>Sending startAgain signal...</source>
        <translation>Enviando señal para escanear de nuevo...</translation>
    </message>
    <message>
        <source>Unknown option: %1</source>
        <translation type="vanished">Opción desconocida: %1</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="163"/>
        <source>Using default language: English.</source>
        <translation>Usando idioma por defecto: Inglés.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="101"/>
        <source>Enable debug log (implicitly enables verbose mode).</source>
        <translation>Activar registro en modo depurar (activa el registro verboso de manera implícita).</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="105"/>
        <source>Discover new Bluetooth devices in order to add them to the trusted list.</source>
        <translation>Buscar nuevos dispositivos Bluetooth para agregarlos a la lista de confianza.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="109"/>
        <source>Where to save log. (Always verbose, debug depends on --debug).</source>
        <translation>Donde guardar los registros (Verboso siempre, depurar depende de --debug).</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="113"/>
        <source>End an existing %1 instance.</source>
        <translation>Termina un instancia de %1 existente.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="117"/>
        <source>Set language. (Available: English (default), and Spanish</source>
        <translation>Establecer idioma (Disponible: Inglés (por defecto), y Español</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="121"/>
        <source>Pause an already running %1 instance.</source>
        <translation>Indicar a una instancia de %1 que ya está corriendo, que se pause.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="125"/>
        <source>Resume an already running and paused %1 instance.</source>
        <translation>Indicar a una instancia de %1, que está corriendo y está en pausa, que se reanude.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="129"/>
        <source>Same as --discover, but for an already running %1 instance.</source>
        <translation>Igual que --discover, pero para una instancia de %1 que ya está corriendo.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="133"/>
        <source>Enable verbose log.</source>
        <translation>Activa el registro en modo verboso.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="157"/>
        <source>Language refers to default language: English.</source>
        <translation>Idioma se refiere al idioma por defecto: Inglés.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="159"/>
        <source>Language: %1 currently not supported.</source>
        <translation>Idioma: %1 actualmente no soportado.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="165"/>
        <source>Using language: %1.</source>
        <translation>Usando idioma: %1.</translation>
    </message>
    <message>
        <source>Using Spanish</source>
        <translation type="vanished">Usando Español</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="174"/>
        <source>Couldn&apos;t register D-Bus service. Won&apos;t be able to respond to IPC messages.
Error message: %1</source>
        <translation>No se pudo registrar el servicio D-BUS. No será posible responder a mensajes IPC.\n
Mensaje de error: %1</translation>
    </message>
</context>
<context>
    <name>ScreenLocker</name>
    <message>
        <location filename="../src/screenlocker.cpp" line="40"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../src/screenlocker.cpp" line="41"/>
        <source>Trusted devices are away, but screen couldn&apos;t be locked.</source>
        <translation>Dispositivos de confianza están lejos, pero no se pudo bloquear la pantalla.</translation>
    </message>
</context>
</TS>
