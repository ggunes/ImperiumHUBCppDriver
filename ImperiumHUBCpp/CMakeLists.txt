cmake_minimum_required(VERSION 3.16)
project(ImperiumGearHUB VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt paketlerini bulma
find_package(Qt6 COMPONENTS Core Gui Widgets SerialPort REQUIRED)

# Kaynak dosyaları
set(SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/devicetab.cpp
    src/ui/keyboardtab.cpp
    src/ui/mousetab.cpp
    src/ui/macroeditor.cpp
    src/ui/profilemanager.cpp
    src/ui/firmwareupdater.cpp
    src/usb/usbmanager.cpp
    src/usb/hiddevice.cpp
    src/profiles/profilemanager.cpp
    src/firmware/firmwaremanager.cpp
)

# Header dosyaları
set(HEADERS
    src/ui/mainwindow.h
    src/ui/devicetab.h
    src/ui/keyboardtab.h
    src/ui/mousetab.h
    src/ui/macroeditor.h
    src/ui/profilemanager.h
    src/ui/firmwareupdater.h
    src/usb/usbmanager.h
    src/usb/hiddevice.h
    src/profiles/profilemanager.h
    src/firmware/firmwaremanager.h
)

# UI dosyaları
set(UI_FILES
    src/ui/mainwindow.ui
    src/ui/devicetab.ui
    src/ui/keyboardtab.ui
    src/ui/mousetab.ui
    src/ui/macroeditor.ui
    src/ui/profilemanager.ui
    src/ui/firmwareupdater.ui
)

# Kaynaklar
set(RESOURCES
    resources/resources.qrc
)

# Yürütülebilir dosya oluşturma
add_executable(ImperiumGearHUB ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCES})

# Kütüphaneleri bağlama
target_link_libraries(ImperiumGearHUB PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
)

# Windows için özel ayarlar
if(WIN32)
    target_link_libraries(ImperiumGearHUB PRIVATE setupapi hid)
endif()

# Derleme çıktı dizinini ayarlama
set_target_properties(ImperiumGearHUB PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Kurulum hedefi (isteğe bağlı)
install(TARGETS ImperiumGearHUB
    RUNTIME DESTINATION bin
)