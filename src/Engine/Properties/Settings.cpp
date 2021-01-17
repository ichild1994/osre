/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2020 OSRE ( Open Source Render Engine ) by Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
#include <cppcore/Common/Variant.h>
#include <osre/Platform/AbstractWindow.h>
#include <osre/Platform/PlatformInterface.h>
#include <osre/Properties/BasePropertyMap.h>
#include <osre/Properties/Property.h>
#include <osre/Properties/Settings.h>

namespace OSRE {
namespace Properties {

using namespace ::OSRE::Platform;

static const CPPCore::Variant EmptyVariant;
static const String ConfigKeyStringTable[Settings::MaxKonfigKey] = {
    "AppName",
    "AppType",
    "AppVersionMajor",
    "AppVersionMinor",
    "AppVersionPatch",
    "WindowsTitle",
    "RenderAPI",
    "WinX",
    "WinY",
    "WinWidth",
    "WinHeight",
    "FSAA",
    "BPP",
    "DepthBufferDepth",
    "StencilBufferDepth",
    "ClearColor",
    "Fullscreen",
    "WindowsResizable",
    "ChildWindow",
    "PollingMode",
    "DefaultFont",
    "RenderMode"
};

Settings::Settings() :
        m_propertyMap(new Properties::BasePropertyMap) {
    initDefaults();
}

Settings::~Settings() {
    delete m_propertyMap;
    m_propertyMap = nullptr;
}

bool Settings::configure(const String &initScriptFile) {
    String filename(initScriptFile);

    return true;
}

bool Settings::setString(Settings::ConfigKey key, const String &strValue) {
    if (strValue.empty() || key >= MaxKonfigKey || key < 0) {
        return false;
    }

    CPPCore::Variant value;
    value.setStdString(strValue);
    m_propertyMap->setProperty(static_cast<ui32>(key), "", value);

    return true;
}

String Settings::getString(ConfigKey key) const {
    const CPPCore::Variant &v = get(key);
    return v.getString();
}

bool Settings::setInt(ConfigKey key, i64 intValue) {
    if (key >= MaxKonfigKey) {
        return false;
    }

    CPPCore::Variant value;
    value.setInt(intValue);
    m_propertyMap->setProperty(key, "", value);

    return true;
}

i64 Settings::getInt(ConfigKey key) const {
    const CPPCore::Variant &v = get(key);
    return v.getInt();
}

bool Settings::setBool(ConfigKey key, bool val) {
    if (key >= MaxKonfigKey) {
        return false;
    }

    CPPCore::Variant value;
    value.setBool(val);
    m_propertyMap->setProperty(key, "", value);

    return true;
}

bool Settings::getBool(ConfigKey key) const {
    const CPPCore::Variant &v = get(key);
    return v.getBool();
}

bool Settings::setFloat(ConfigKey key, f32 floatValue) {
    if (key >= MaxKonfigKey) {
        return false;
    }
    CPPCore::Variant value;
    value.setFloat(floatValue);
    m_propertyMap->setProperty(key, "", value);

    return true;
}

f32 Settings::getFloat(ConfigKey key) const {
    const CPPCore::Variant &v = get(key);
    return v.getFloat();
}

const CPPCore::Variant &Settings::get(ConfigKey key) const {
    Properties::Property *pProperty = m_propertyMap->getProperty(key);
    if (!pProperty) {
        return EmptyVariant;
    }

    return pProperty->getValue();
}

const String &Settings::getKeyAsString(ConfigKey key) const {
    return ConfigKeyStringTable[key];
}

void Settings::clear() {
    if (nullptr != m_propertyMap) {
        m_propertyMap->clear();
    }
}

static i64 mapPlatformtype2Int(PluginType type) {
#ifdef OSRE_WINDOWS
    if (PluginType::WindowsPlugin == type) {
        return 0;
    }
#else
    if (PluginType::SDL2Plugin == type) {
        return 1;
    }
#endif
    return -1;
}

void Settings::initDefaults() {
    CPPCore::Variant appName, windowsTitle, renderAPI, value;
    appName.setStdString("OSRE Application");
    m_propertyMap->setProperty(AppName, ConfigKeyStringTable[AppName], appName);

    value.setInt(static_cast<i64>(GfxApp));
    m_propertyMap->setProperty(AppType, ConfigKeyStringTable[AppName], value);

    // Set version 0.1.0 as default
    value.setInt(0);
    m_propertyMap->setProperty(AppVersionMajor, ConfigKeyStringTable[AppVersionMajor], value);
    value.setInt(1);
    m_propertyMap->setProperty(AppVersionMajor, ConfigKeyStringTable[AppVersionMinor], value);
    value.setInt(0);
    m_propertyMap->setProperty(AppVersionPatch, ConfigKeyStringTable[AppVersionPatch], value);

    windowsTitle.setStdString("The OSRE experience");
    const i64 pluginType( mapPlatformtype2Int( Platform::PlatformInterface::getOSPluginType() ) );
    value.setInt( pluginType );
    m_propertyMap->setProperty( WindowsTitle, ConfigKeyStringTable[ WindowsTitle ], windowsTitle );
    renderAPI.setStdString("opengl");
    m_propertyMap->setProperty( RenderAPI, ConfigKeyStringTable[ RenderAPI ], renderAPI );

    value.setInt( 0 );
    m_propertyMap->setProperty( WinX, ConfigKeyStringTable[ WinX ], value );
    m_propertyMap->setProperty( WinY, ConfigKeyStringTable[ WinY ], value );
    value.setInt( 1024 );
    m_propertyMap->setProperty( WinWidth, ConfigKeyStringTable[ WinWidth ], value );
    value.setInt( 768 );
    m_propertyMap->setProperty( WinHeight, ConfigKeyStringTable[ WinHeight ], value );

    value.setBool( true );
    m_propertyMap->setProperty( WindowsResizable, ConfigKeyStringTable[ WindowsResizable ], value );
    value.setBool( false );
    m_propertyMap->setProperty( ChildWindow, ConfigKeyStringTable[ ChildWindow ], value );
    value.setInt( 2 );
    m_propertyMap->setProperty( FSAA, ConfigKeyStringTable[ FSAA ], value );
    value.setInt( 32 );
    m_propertyMap->setProperty( BPP, ConfigKeyStringTable[ BPP ], value );
    value.setInt( 24 );
    m_propertyMap->setProperty( DepthBufferDepth, ConfigKeyStringTable[ DepthBufferDepth ], value );
    m_propertyMap->setProperty( StencilBufferDepth, ConfigKeyStringTable[ StencilBufferDepth ], value ); 
    
    value.setFloat4( 0, 0,0 ,0 );
    m_propertyMap->setProperty( ClearColor, ConfigKeyStringTable[ ClearColor ], value );
    value.setBool( false );
    m_propertyMap->setProperty( PollingMode, ConfigKeyStringTable[ PollingMode ], value );

    value.setStdString( "buildin_arial.bmp" );
    m_propertyMap->setProperty( DefaultFont, ConfigKeyStringTable[ DefaultFont ], value );

    value.setInt( 1 );
    m_propertyMap->setProperty( RenderMode, ConfigKeyStringTable[ RenderMode], value );
}

} // Namespace Properties
} // Namespace OSRE
