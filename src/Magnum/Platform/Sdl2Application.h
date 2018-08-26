#ifndef Magnum_Platform_Sdl2Application_h
#define Magnum_Platform_Sdl2Application_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::Platform::Sdl2Application, macro @ref MAGNUM_SDL2APPLICATION_MAIN()
 */

#include <memory>
#include <Corrade/Corrade.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Platform/Platform.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/GL.h"
#endif

#ifdef CORRADE_TARGET_WINDOWS /* Windows version of SDL2 redefines main(), we don't want that */
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>
#include <SDL_scancode.h>

#ifdef CORRADE_TARGET_WINDOWS_RT
#include <SDL_main.h> /* For SDL_WinRTRunApp */
#include <wrl.h> /* For the WinMain entrypoint */
#endif

namespace Magnum { namespace Platform {

namespace Implementation {
    enum class DpiScalingPolicy: UnsignedByte;
}

/** @nosubgrouping
@brief SDL2 application

Application using [Simple DirectMedia Layer](http://www.libsdl.org/) toolkit.
Supports keyboard and mouse handling.

This application library is in theory available for all platforms for which
SDL2 is ported (thus also @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", see
respective sections in @ref building-corrade-cross-emscripten "Corrade's" and
@ref building-cross-emscripten "Magnum's" building documentation). It depends
on the [SDL2](http://www.libsdl.org) library (Emscripten has it built in) and
is built if `WITH_SDL2APPLICATION` is enabled in CMake.

@section Platform-Sdl2Application-bootstrap Bootstrap application

Fully contained base application using @ref Sdl2Application along with
CMake setup is available in `base` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base.zip) file.
After extracting the downloaded archive you can build and run the application
with these four commands:

@code{.sh}
mkdir build && cd build
cmake ..
cmake --build .
./src/MyApplication # or ./src/Debug/MyApplication
@endcode

See @ref cmake for more information.

@section Platform-Sdl2Application-bootstrap-emscripten Bootstrap application for Emscripten

Fully contained base application using @ref Sdl2Application for both desktop
and Emscripten build along with full HTML markup and CMake setup is available
in `base-emscripten` branch of [Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap)
repository, download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-emscripten.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-emscripten.zip)
file. After extracting the downloaded archive, you can do the desktop build in
the same way as above. For the Emscripten build you also need to put the
contents of toolchains repository from https://github.com/mosra/toolchains
in `toolchains/` subdirectory. There are two toolchain files. The
`generic/Emscripten.cmake` is for the classical (asm.js) build, the
`generic/Emscripten-wasm.cmake` is for WebAssembly build. Don't forget to adapt
`EMSCRIPTEN_PREFIX` variable in `toolchains/generic/Emscripten*.cmake` to path
where Emscripten is installed; you can also pass it explicitly on command-line
using `-DEMSCRIPTEN_PREFIX`. Default is `/usr/emscripten`.

Then create build directory and run `cmake` and build/install commands in it.
Set `CMAKE_PREFIX_PATH` to where you have all the dependencies installed, set
`CMAKE_INSTALL_PREFIX` to have the files installed in proper location (a
webserver, e.g.  `/srv/http/emscripten`).

@code{.sh}
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Emscripten.cmake" \
    -DCMAKE_PREFIX_PATH=/usr/lib/emscripten/system \
    -DCMAKE_INSTALL_PREFIX=/srv/http/emscripten
cmake --build .
cmake --build . --target install
@endcode

You can then open `MyApplication.html` in your browser (through a webserver,
e.g. http://localhost/emscripten/MyApplication.html).

Detailed information about deployment for Emscripten and all needed boilerplate
together with a troubleshooting guide is available in @ref platforms-html5.

@section Platform-Sdl2Application-bootstrap-ios Bootstrap application for iOS

Fully contained base application using @ref Sdl2Application for both desktop
and iOS build along with pre-filled `*.plist` is available in `base-ios` branch
of [Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-ios.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-ios.zip) file.
After extracting the downloaded archive, you can do the desktop build in
the same way as above. For the iOS build you also need to put the contents of
toolchains repository from https://github.com/mosra/toolchains in `toolchains/`
subdirectory.

Then create build directory and run `cmake` to generate the Xcode project. Set
`CMAKE_OSX_ROOT` to SDK you want to target and enable all desired architectures
in `CMAKE_OSX_ARCHITECTURES`. Set `CMAKE_PREFIX_PATH` to the directory where
you have all the dependencies.

@code{.sh}
mkdir build-ios && cd build-ios
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk \
    -DCMAKE_OSX_ARCHITECTURES="arm64;armv7;armv7s" \
    -DCMAKE_PREFIX_PATH=~/ios-libs \
    -G Xcode
@endcode

You can then open the generated project file in Xcode and build/deploy it from
there.

@section Platform-Sdl2Application-bootstrap-winrt Bootstrap application for Windows RT

Fully contained base application using @ref Sdl2Application for both desktop
and Windows Phone / Windows Store build along with all required plumbing is
available in `base-winrt` branch of [Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap)
repository, download it as [zip](https://github.com/mosra/magnum-bootstrap/archive/base-winrt.zip)
file. After extracting the downloaded archive, you can do the desktop build in
the same way as above.

For the Windows RT build you need to provide [your own *.pfx certificate file](https://msdn.microsoft.com/en-us/library/windows/desktop/jj835832.aspx) and
pass it to CMake in a `SIGNING_CERTIFICATE` variable. The bootstrap application
assumes that SDL2 and ANGLE is built as DLL and both Corrade and Magnum are
built statically. Assuming the native Corrade installation is in `C:/Sys` and
all WinRT dependencies are in `C:/Sys-winrt`, the build can be done similarly
to the following:

@code{.bat}
mkdir build-winrt && cd build-winrt
cmake .. ^
    -DCORRADE_RC_EXECUTABLE="C:/Sys/bin/corrade-rc.exe" ^
    -DCMAKE_PREFIX_PATH="C:/Sys-winrt" ^
    -DCMAKE_SYSTEM_NAME=WindowsStore ^
    -DCMAKE_SYSTEM_VERSION=8.1 ^
    -G "Visual Studio 14 2015" ^
    -DSIGNING_CERTIFICATE=<path-to-your-pfx-file>
cmake --build .
@endcode

Change `WindowsStore` to `WindowsPhone` if you want to build for Windows Phone
instead. The `build-winrt/src/AppPackages` directory will then contain the
final package along with a PowerShell script for easy local installation.

@section Platform-Sdl2Application-usage General usage

In order to use this library from CMake, you need to copy `FindSDL2.cmake` from
the `modules/` directory in Magnum source to the `modules/` dir in your project
(so it is able to find the SDL2 library). In case of Emscripten you need also
`FindOpenGLES2.cmake` / `FindOpenGLES3.cmake`. Request the `Sdl2Application`
component of the `Magnum` package and link to the `Magnum::Sdl2Application`
target:

@code{.cmake}
find_package(Magnum REQUIRED Sdl2Application)

# ...
target_link_libraries(your-app Magnum::Sdl2Application)
@endcode

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. Again, see @ref building and
@ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in `main()` --- see
convenience macro @ref MAGNUM_SDL2APPLICATION_MAIN(). See @ref platform for
more information.

@code{.cpp}
class MyApplication: public Platform::Sdl2Application {
    // implement required methods...
};
MAGNUM_SDL2APPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::Application @ce and the macro is aliased to @cpp MAGNUM_APPLICATION_MAIN() @ce
to simplify porting.

@subsection Platform-Sdl2Application-usage-ios iOS specifics

Leaving a default (zero) window size in @ref Configuration will cause the app
to autodetect it based on the actual device screen size. This also depends on
@ref Platform-Sdl2Application-dpi "DPI awareness", see below for details.

As noted in the @ref platforms-ios-bundle "iOS platform guide", a lot of
options needs to be set via a `*.plist` file. Some options can be configured
from runtime when creating the SDL2 application window, see documentation of
a particular value for details:

-   @ref Configuration::WindowFlag::Borderless hides the menu bar
-   @ref Configuration::WindowFlag::Resizable makes the application respond to
    device orientation changes

@subsection Platform-Sdl2Application-usage-emscripten Emscripten specifics

Leaving a default (zero) window size in @ref Configuration will cause the app
to use a window size that corresponds to *CSS pixel size* of the
@cb{.html} <canvas> @ce element. The size is then multiplied by DPI scaling
value, see @ref Platform-Sdl2Application-dpi "DPI awareness" below for details.

If you enable @ref Configuration::WindowFlag::Resizable, the canvas will be
resized when size of the canvas changes and you get @ref viewportEvent(). If
the flag is not enabled, no canvas resizing is performed.

@section Platform-Sdl2Application-dpi DPI awareness

On displays that match the platform default DPI (96 or 72),
@ref Configuration::setSize() will create the window in exactly the requested
size and the framebuffer pixels will match display pixels 1:1. On displays that
have different DPI, there are three possible scenarios, listed below. It's
possible to fine tune the behavior either using extra parameters passed to
@ref Configuration::setSize() or via the `--magnum-dpi-scaling` command-line
option.

-   Framebuffer DPI scaling. The window is created with exactly the requested
    size and all event coordinates are reported also relative to that size.
    However, the window backing framebuffer has a different size. This is only
    supported on macOS and iOS. See @ref platforms-macos-hidpi for details how
    to enable it. Equivalent to passing
    @ref Configuration::DpiScalingPolicy::Framebuffer to
    @ref Configuration::setSize() or `framebuffer` on command line.
-   Virtual DPI scaling. Scales the window based on DPI scaling setting in the
    system. For example if a 800x600 window is requested and DPI scaling is set
    to 200%, the resulting window will have 1600x1200 pixels. The backing
    framebuffer will have the same size. This is supported on Linux and
    Windows. Equivalent to passing @ref Configuration::DpiScalingPolicy::Virtual
    to @ref Configuration::setSize() or `virtual` on command line.
-   Physical DPI scaling. Takes the requested window size as a physical size
    that a window would have on platform's default DPI and scales it to have
    the same physical size on given display physical DPI. So, for example on a
    display with 240 DPI the window size will be 2000x1500 in pixels, but it
    will be 21 centimeters wide, the same as a 800x600 window would be on a 96
    DPI display. On platforms that don't have a concept of a window (such
    as mobile platforms or @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten"), it
    causes the framebuffer to match display pixels 1:1 without any scaling.
    This is supported on Linux, Windows, all mobile platforms except iOS and
    Emscripten. Equivalent to passing
    @ref Configuration::DpiScalingPolicy::Physical to
    @ref Configuration::setSize() or `physical` on command line.

Besides the above, it's possible to supply a custom DPI scaling value to
@ref Configuration::setSize() or the `--magnum-dpi-scaling` command-line
option. Using `--magnum-dpi-scaling &lt;number&gt;` will make the scaling
same in both directions, with `--magnum-dpi-scaling "<horizontal> <vertical>"`
the scaling will be different in each direction. On desktop systems custom DPI
scaling value will affect physical window size (with the content being scaled),
on mobile and web it will affect sharpness of the contents.

The default is depending on the platform:

-   On macOS and iOS, the default and only supported option is
    @ref Configuration::DpiScalingPolicy::Framebuffer. On this platform,
    @ref windowSize() and @ref framebufferSize() will differ depending on
    whether `NSHighResolutionCapable` is enabled in the `*.plist` file or not.
    By default, @ref dpiScaling() is @cpp 1.0f @ce in both dimensions but it
    can be overriden using custom DPI scaling.
-   On Windows, the default is @ref Configuration::DpiScalingPolicy::Framebuffer.
    The @ref windowSize() and @ref framebufferSize() is always the same.
    Depending on whether the DPI awareness was enabled in the manifest file or
    set by the `SetProcessDpiAwareness()` API, @ref dpiScaling() is either
    @cpp 1.0f @ce in both dimensions, indicating a low-DPI screen or a
    non-DPI-aware app, or some other value for HiDPI screens. In both cases the
    value can be overriden using custom DPI scaling.
-   On Linux, the default is @ref Configuration::DpiScalingPolicy::Virtual,
    taken from the `Xft.dpi` property. If the property is not available, it
    falls back to @ref Configuration::DpiScalingPolicy::Physical, querying the
    monitor DPI value. The @ref windowSize() and @ref framebufferSize() is
    always the same, @ref dpiScaling() contains the queried DPI scaling value.
    The value can be overriden using custom DPI scaling.
-   On @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", the default is physical DPI
    scaling, taken from [Window.getDevicePixelRatio()](https://developer.mozilla.org/en-US/docs/Web/API/Window/devicePixelRatio). The
    @ref windowSize() and @ref framebufferSize() is always the same,
    @ref dpiScaling() contains the queried DPI scaling value. The value can be
    overriden using custom DPI scaling.

If your application is saving and restoring window size, it's advisable to take
@ref dpiScaling() into account:

-   Either divide the window size by the DPI scaling value and use that to
    restore the window next time --- but note this might accumulate slight
    differences in window sizes over time, especially if fractional scaling is
    involved.
-   Or save the scaled size and use @ref Configuration::setSize(const Vector2i&, const Vector2&)
    with @cpp 1.0f @ce as custom DPI scaling next time --- but this doesn't
    properly handle cases where the window is opened on a display with
    different DPI.
*/
class Sdl2Application {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv} {}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        class Configuration;
        #ifdef MAGNUM_TARGET_GL
        class GLConfiguration;
        #endif
        class ViewportEvent;
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;
        class MouseScrollEvent;
        class MultiGestureEvent;
        class TextInputEvent;
        class TextEditingEvent;

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Construct with given configuration for OpenGL context
         * @param arguments         Application arguments
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreate() for an
         * alternative.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         */
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Construct with given configuration
         *
         * If @ref Configuration::WindowFlag::Contextless is present or Magnum
         * was not built with @ref MAGNUM_TARGET_GL, this creates a window
         * without any GPU context attached, leaving that part on the user.
         *
         * If none of the flags is present and Magnum was built with
         * @ref MAGNUM_TARGET_GL, this is equivalent to calling
         * @ref Sdl2Application(const Arguments&, const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         *
         * See also @ref building-features for more information.
         */
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration);

        /**
         * @brief Construct with default configuration
         *
         * Equivalent to calling @ref Sdl2Application(const Arguments&, const Configuration&)
         * with default-constructed @ref Configuration.
         */
        explicit Sdl2Application(const Arguments& arguments);

        /**
         * @brief Construct without creating a window
         * @param arguments     Application arguments
         *
         * Unlike above, the window is not created and must be created later
         * with @ref create() or @ref tryCreate().
         */
        explicit Sdl2Application(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief Sdl2Application(const Arguments&, NoCreateT)
         * @deprecated Use @ref Sdl2Application(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use Sdl2Application(const Arguments&, NoCreateT) instead") explicit Sdl2Application(const Arguments& arguments, std::nullptr_t): Sdl2Application{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        Sdl2Application(const Sdl2Application&) = delete;

        /** @brief Moving is not allowed */
        Sdl2Application(Sdl2Application&&) = delete;

        /** @brief Copying is not allowed */
        Sdl2Application& operator=(const Sdl2Application&) = delete;

        /** @brief Moving is not allowed */
        Sdl2Application& operator=(Sdl2Application&&) = delete;

        /**
         * @brief Execute main loop
         * @return Value for returning from @cpp main() @ce
         *
         * Calls @ref mainLoopIteration() in a loop until @ref exit() is
         * called. See @ref MAGNUM_SDL2APPLICATION_MAIN() for usage
         * information.
         */
        int exec();

        /**
         * @brief Exit application main loop
         *
         * Stops main loop started by @ref exec().
         */
        void exit();

        /**
         * @brief Run one iteration of application main loop
         *
         * Called internally from @ref exec(). If you want to have better
         * control over how the main loop behaves, you can call this function
         * yourself from your own `main()` function instead of it being called
         * automatically from @ref exec() / @ref MAGNUM_SDL2APPLICATION_MAIN().
         */
        void mainLoopIteration();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Underlying window handle
         *
         * Use in case you need to call SDL functionality directly.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        SDL_Window* window() { return _window; }
        #endif

    protected:
        /* Nobody will need to have (and delete) Sdl2Application*, thus this is
           faster than public pure virtual destructor */
        ~Sdl2Application();

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Create a window with given configuration for OpenGL context
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Must be called only if the context wasn't created by the constructor
         * itself, i.e. when passing @ref NoCreate to it. Error message is
         * printed and the program exits if the context cannot be created, see
         * @ref tryCreate() for an alternative.
         *
         * On desktop GL, if version is not specified in @p glConfiguration,
         * the application first tries to create core context (OpenGL 3.2+ on
         * macOS, OpenGL 3.1+ elsewhere) and if that fails, falls back to
         * compatibility OpenGL 2.1 context.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         */
        void create(const Configuration& configuration, const GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Create a window with given configuration
         *
         * If @ref Configuration::WindowFlag::Contextless is present or Magnum
         * was not built with @ref MAGNUM_TARGET_GL, this creates a window
         * without any GPU context attached, leaving that part on the user.
         *
         * If none of the flags is present and Magnum was built with
         * @ref MAGNUM_TARGET_GL, this is equivalent to calling
         * @ref create(const Configuration&, const GLConfiguration&) with
         * default-constructed @ref GLConfiguration.
         *
         * See also @ref building-features for more information.
         */
        void create(const Configuration& configuration);

        /**
         * @brief Create a window with default configuration and OpenGL context
         *
         * Equivalent to calling @ref create(const Configuration&) with
         * default-constructed @ref Configuration.
         */
        void create();

        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        /** @brief @copybrief create(const Configuration&, const GLConfiguration&)
         * @deprecated Use @ref create(const Configuration&, const GLConfiguration&) instead.
         */
        CORRADE_DEPRECATED("use create(const Configuration&, const GLConfiguration&) instead") void createContext(const Configuration& configuration) {
            create(configuration);
        }

        /** @brief @copybrief create()
         * @deprecated Use @ref create() instead.
         */
        CORRADE_DEPRECATED("use create() instead") void createContext() {
            create();
        }
        #endif

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Try to create context with given configuration for OpenGL context
         *
         * Unlike @ref create(const Configuration&, const GLConfiguration&)
         * returns @cpp false @ce if the context cannot be created,
         * @cpp true @ce otherwise.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         */
        bool tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Try to create context with given configuration
         *
         * Unlike @ref create(const Configuration&) returns @cpp false @ce if
         * the context cannot be created, @cpp true @ce otherwise.
         */
        bool tryCreate(const Configuration& configuration);

        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        /** @brief @copybrief tryCreate(const Configuration&, const GLConfiguration&)
         * @deprecated Use @ref tryCreate(const Configuration&, const GLConfiguration&) instead.
         */
        CORRADE_DEPRECATED("use tryCreate(const Configuration&, const GLConfiguration&) instead") bool tryCreateContext(const Configuration& configuration) {
            return tryCreate(configuration);
        }
        #endif

        /** @{ @name Screen handling */

    public:
        /**
         * @brief Window size
         *
         * Window size to which all input event coordinates can be related.
         * Note that, especially on HiDPI systems, it may be different from
         * @ref framebufferSize(). If a window is not created yet, returns
         * zero vector. See @ref Platform-Sdl2Application-dpi for more
         * information.
         * @see @ref dpiScaling()
         */
        Vector2i windowSize() const;

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * Size of the default framebuffer. Note that, especially on HiDPI
         * systems, it may be different from @ref windowSize(). If a window is
         * not created yet, returns zero vector. See
         * @ref Platform-Sdl2Application-dpi for more information.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         *
         * @see @ref dpiScaling()
         */
        Vector2i framebufferSize() const;
        #endif

        /**
         * @brief DPI scaling
         *
         * How the content should be scaled relative to system defaults for
         * given @ref windowSize(). If a window is not created yet, returns
         * zero vector, use @ref dpiScaling(const Configuration&) const for
         * calculating a value independently. See @ref Platform-Sdl2Application-dpi
         * for more information.
         * @see @ref framebufferSize()
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /**
         * @brief DPI scaling for given configuration
         *
         * Calculates DPI scaling that would be used when creating a window
         * with given @p configuration. Takes into account DPI scaling policy
         * and custom scaling specified on the command-line. See
         * @ref Platform-Sdl2Application-dpi for more information.
         */
        Vector2 dpiScaling(const Configuration& configuration) const;

        #if defined(CORRADE_TARGET_EMSCRIPTEN) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Set container CSS class
         *
         * Assigns given CSS class to the @cb{.html} <div class="container"> @ce.
         * Useful for example to change aspect ratio of the view or stretch it
         * to cover the full page. See @ref platforms-html5-layout for more
         * information about possible values. Note that this replaces any
         * existing class, to set multiple classes separate them with
         * whitespace.
         *
         * @note Only available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        void setContainerCssClass(const std::string& cssClass);
        #endif

    protected:
        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         * @see @ref setSwapInterval()
         */
        void swapBuffers();

        /** @brief Swap interval */
        Int swapInterval() const;

        /**
         * @brief Set swap interval
         *
         * Set @cpp 0 @ce for no VSync, @cpp 1 @ce for enabled VSync. Some
         * platforms support @cpp -1 @ce for late swap tearing. Prints error
         * message and returns @cpp false @ce if swap interval cannot be set,
         * @cpp true @ce otherwise. Default is driver-dependent, you can query
         * the value with @ref swapInterval().
         * @see @ref setMinimalLoopPeriod()
         */
        bool setSwapInterval(Int interval);

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Set minimal loop period
         *
         * This setting reduces the main loop frequency in case VSync is
         * not/cannot be enabled or no drawing is done. Default is @cpp 0 @ce
         * (i.e. looping at maximum frequency).
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten",
         *      the browser is managing the frequency instead.
         * @see @ref setSwapInterval()
         */
        void setMinimalLoopPeriod(UnsignedInt milliseconds) {
            _minimalLoopPeriod = milliseconds;
        }
        #endif

        /**
         * @brief Redraw immediately
         *
         * Marks the window for redrawing, resulting in call to @ref drawEvent()
         * in the next iteration. You can call it from @ref drawEvent() itself
         * to redraw immediately without waiting for user input.
         */
        void redraw() { _flags |= Flag::Redraw; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Tick event
         *
         * If implemented, this function is called periodically after
         * processing all input events and before draw event even though there
         * might be no input events and redraw is not requested. Useful e.g.
         * for asynchronous task polling. Use @ref setMinimalLoopPeriod()/
         * @ref setSwapInterval() to control main loop frequency.
         */
        virtual void tickEvent();

        /**
         * @brief Viewport event
         *
         * Called when window size changes. The default implementation does
         * nothing. If you want to respond to size changes, you should pass the
         * new *framebuffer* size to @ref GL::DefaultFramebuffer::setViewport()
         * (if using OpenGL) and possibly elsewhere (to
         * @ref SceneGraph::Camera::setViewport(), other framebuffers...) and
         * the new *window* size and DPI scaling to APIs that respond to user
         * events or scale UI elements.
         *
         * Note that this function might not get called at all if the window
         * size doesn't change. You should configure the initial state of your
         * cameras, framebuffers etc. in application constructor rather than
         * relying on this function to be called. Size of the window can be
         * retrieved using @ref windowSize(), size of the backing framebuffer
         * via @ref framebufferSize() and DPI scaling using @ref dpiScaling().
         * See @ref Platform-Sdl2Application-dpi for detailed info about these
         * values.
         */
        virtual void viewportEvent(ViewportEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief viewportEvent(ViewportEvent&)
         * @deprecated Use @ref viewportEvent(ViewportEvent&) instead.
         *      To preserve backwards compatibility, this function is called
         *      from @ref viewportEvent(ViewportEvent&) with
         *      @ref ViewportEvent::framebufferSize() passed to @p size.
         *      Overriding the new function will cause this function to not be
         *      called anymore.
         */
        virtual CORRADE_DEPRECATED("use viewportEvent(ViewportEvent&) instead") void viewportEvent(const Vector2i& size);
        #endif

        /**
         * @brief Draw event
         *
         * Called when the screen is redrawn. You should clean the framebuffer
         * using @ref GL::DefaultFramebuffer::clear() (if using OpenGL) and
         * then add your own drawing functions. After drawing is finished, call
         * @ref swapBuffers(). If you want to draw immediately again, call also
         * @ref redraw().
         */
        virtual void drawEvent() = 0;

        /*@}*/

        /** @{ @name Keyboard handling */

        /**
         * @brief Key press event
         *
         * Called when an key is pressed. Default implementation does nothing.
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when an key is released. Default implementation does nothing.
         */
        virtual void keyReleaseEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /** @brief Whether mouse is locked */
        bool isMouseLocked() const { return SDL_GetRelativeMouseMode(); }

        /**
         * @brief Enable or disable mouse locking
         *
         * When mouse is locked, the cursor is hidden and only
         * @ref MouseMoveEvent::relativePosition() is changing, absolute
         * position stays the same.
         */
        void setMouseLocked(bool enabled);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Mouse press event
         *
         * Called when mouse button is pressed. Default implementation does
         * nothing.
         */
        virtual void mousePressEvent(MouseEvent& event);

        /**
         * @brief Mouse release event
         *
         * Called when mouse button is released. Default implementation does
         * nothing.
         */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Mouse move event
         *
         * Called when mouse is moved. Default implementation does nothing.
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /**
         * @brief Mouse scroll event
         *
         * Called when a scrolling device is used (mouse wheel or scrolling
         * area on a touchpad). Default implementation does nothing.
         */
        virtual void mouseScrollEvent(MouseScrollEvent& event);

        /*@}*/

        /** @{ @name Touch gesture handling */

        /**
         * @brief Multi gesture event
         *
         * Called when the user performs a gesture using multiple fingers.
         * Default implementation does nothing.
         * @experimental
         */
        virtual void multiGestureEvent(MultiGestureEvent& event);

        /*@}*/

        /** @{ @name Text input handling */
    public:
        /**
         * @brief Whether text input is active
         *
         * If text input is active, text input events go to @ref textInputEvent()
         * and @ref textEditingEvent().
         * @note Note that in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" the
         *      value is emulated and might not reflect external events like
         *      closing on-screen keyboard.
         * @see @ref startTextInput(), @ref stopTextInput()
         */
        bool isTextInputActive();

        /**
         * @brief Start text input
         *
         * Starts text input that will go to @ref textInputEvent() and
         * @ref textEditingEvent().
         * @see @ref stopTextInput(), @ref isTextInputActive(),
         *      @ref setTextInputRect()
         */
        void startTextInput();

        /**
         * @brief Stop text input
         *
         * Stops text input that went to @ref textInputEvent() and
         * @ref textEditingEvent().
         * @see @ref startTextInput(), @ref isTextInputActive(), @ref textInputEvent()
         *      @ref textEditingEvent()
         */
        void stopTextInput();

        /**
         * @brief Set text input rectangle
         *
         * The @p rect defines an area where the text is being displayed, for
         * example to hint the system where to place on-screen keyboard.
         */
        void setTextInputRect(const Range2Di& rect);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Text input event
         *
         * Called when text input is active and the text is being input.
         * @see @ref isTextInputActive()
         */
        virtual void textInputEvent(TextInputEvent& event);

        /**
         * @brief Text editing event
         *
         * Called when text input is active and the text is being edited.
         */
        virtual void textEditingEvent(TextEditingEvent& event);

        /*@}*/

    private:
        enum class Flag: UnsignedByte {
            Redraw = 1 << 0,
            VSyncEnabled = 1 << 1,
            NoTickEvent = 1 << 2,
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            Exit = 1 << 3
            #endif
            #ifdef CORRADE_TARGET_EMSCRIPTEN
            TextInputActive = 1 << 4,
            Resizable = 1 << 5
            #endif
        };

        typedef Containers::EnumSet<Flag> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        /* These are saved from command-line arguments */
        bool _verboseLog{};
        Implementation::DpiScalingPolicy _commandLineDpiScalingPolicy{};
        Vector2 _commandLineDpiScaling;

        Vector2 _dpiScaling;

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_Window* _window{};
        UnsignedInt _minimalLoopPeriod;
        #else
        Vector2i _lastKnownCanvasSize;
        #endif

        #ifdef MAGNUM_TARGET_GL
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_GLContext _glContext;
        #else
        SDL_Surface* _glContext{};
        #endif
        std::unique_ptr<Platform::GLContext> _context;
        #endif

        Flags _flags;
};

#ifdef MAGNUM_TARGET_GL
/**
@brief OpenGL context configuration

The created window is always with double-buffered OpenGL context.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref Sdl2Application(), @ref create(), @ref tryCreate()
*/
class Sdl2Application::GLConfiguration {
    public:
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Context flag
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref Flags, @ref setFlags(), @ref Context::Flag
         * @todo re-enable when Emscripten has proper SDL2 support
         */
        enum class Flag: int {
            Debug = SDL_GL_CONTEXT_DEBUG_FLAG,  /**< Create debug context */

            /** Create context with robust access */
            RobustAccess = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG,

            /** Create context with reset isolation */
            ResetIsolation = SDL_GL_CONTEXT_RESET_ISOLATION_FLAG
        };

        /**
         * @brief Context flags
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref setFlags(), @ref Context::Flags
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<Flag, SDL_GL_CONTEXT_DEBUG_FLAG|
            SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG|SDL_GL_CONTEXT_RESET_ISOLATION_FLAG> Flags;
        #else
        typedef Containers::EnumSet<Flag> Flags;
        #endif
        #endif

        explicit GLConfiguration();
        ~GLConfiguration();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Context flags
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is no flag. See also @ref GL::Context::flags().
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        GLConfiguration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        /**
         * @brief Context version
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        GL::Version version() const { return _version; }
        #endif

        /**
         * @brief Set context version
         *
         * If requesting version greater or equal to OpenGL 3.1, core profile
         * is used. The created context will then have any version which is
         * backwards-compatible with requested one. Default is
         * @ref GL::Version::None, i.e. any provided version is used.
         * @note In @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" this function
         *      does nothing (@ref GL::Version::GLES200 or
         *      @ref GL::Version::GLES300 is used implicitly based on the
         *      target).
         */
        GLConfiguration& setVersion(GL::Version version) {
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            _version = version;
            #else
            static_cast<void>(version);
            #endif
            return *this;
        }

        /** @brief Color buffer size */
        Vector4i colorBufferSize() const { return _colorBufferSize; }

        /**
         * @brief Set color buffer size
         *
         * Default is @cpp {8, 8, 8, 0} @ce (8-bit-per-channel RGB, no alpha).
         * @see @ref setDepthBufferSize(), @ref setStencilBufferSize()
         */
        GLConfiguration& setColorBufferSize(const Vector4i& size) {
            _colorBufferSize = size;
            return *this;
        }

        /** @brief Depth buffer size */
        Int depthBufferSize() const { return _depthBufferSize; }

        /**
         * @brief Set depth buffer size
         *
         * Default is @cpp 24 @ce bits.
         * @see @ref setColorBufferSize(), @ref setStencilBufferSize()
         */
        GLConfiguration& setDepthBufferSize(Int size) {
            _depthBufferSize = size;
            return *this;
        }

        /** @brief Stencil buffer size */
        Int stencilBufferSize() const { return _stencilBufferSize; }

        /**
         * @brief Set stencil buffer size
         *
         * Default is @cpp 0 @ce bits (i.e., no stencil buffer).
         * @see @ref setColorBufferSize(), @ref setDepthBufferSize()
         */
        GLConfiguration& setStencilBufferSize(Int size) {
            _stencilBufferSize = size;
            return *this;
        }

        /** @brief Sample count */
        Int sampleCount() const { return _sampleCount; }

        /**
         * @brief Set sample count
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 0 @ce, thus no multisampling. See also
         * @ref GL::Renderer::Feature::Multisampling.
         */
        GLConfiguration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief sRGB-capable default framebuffer
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        bool isSRGBCapable() const { return _sRGBCapable; }

        /**
         * @brief Set sRGB-capable default framebuffer
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp false @ce. See also
         * @ref GL::Renderer::Feature::FramebufferSRGB.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        GLConfiguration& setSRGBCapable(bool enabled) {
            _sRGBCapable = enabled;
            return *this;
        }
        #endif

    private:
        Vector4i _colorBufferSize;
        Int _depthBufferSize, _stencilBufferSize;
        Int _sampleCount;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        GL::Version _version;
        Flags _flags;
        bool _sRGBCapable;
        #endif
};

#ifndef CORRADE_TARGET_EMSCRIPTEN
CORRADE_ENUMSET_OPERATORS(Sdl2Application::GLConfiguration::Flags)
#endif
#endif

namespace Implementation {
    enum class DpiScalingPolicy: UnsignedByte {
        /* Using 0 for an "unset" value */

        #ifdef CORRADE_TARGET_APPLE
        Framebuffer = 1,
        #endif

        #ifndef CORRADE_TARGET_APPLE
        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
        Virtual = 2,
        #endif

        Physical = 3,
        #endif

        Default
            #ifdef CORRADE_TARGET_APPLE
            = Framebuffer
            #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
            = Virtual
            #else
            = Physical
            #endif
    };
}

/**
@brief Configuration

@see @ref Sdl2Application(), @ref GLConfiguration, @ref create(),
    @ref tryCreate()
*/
class Sdl2Application::Configuration {
    public:
        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL) && !defined(CORRADE_TARGET_EMSCRIPTEN)
        /** @brief @copybrief GLConfiguration::Flag
         * @deprecated Use @ref GLConfiguration::Flag instead.
         */
        typedef GLConfiguration::Flag Flag;

        /** @brief @copybrief GLConfiguration::Flags
         * @deprecated Use @ref GLConfiguration::Flags instead.
         */
        typedef GLConfiguration::Flags Flags;
        #endif

        /**
         * @brief Window flag
         *
         * @see @ref WindowFlags, @ref setWindowFlags()
         */
        enum class WindowFlag: Uint32 {
            /**
             * Resizable window. On iOS this allows the application to respond
             * to display orientation changes, on
             * @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" this causes the
             * framebuffer to be resized when the @cb{.html} <canvas> @ce size
             * changes.
             *
             * Implement @ref viewportEvent() to react to the resizing events.
             */
            Resizable = SDL_WINDOW_RESIZABLE,

            #ifndef CORRADE_TARGET_EMSCRIPTEN
            /**
             * Fullscreen window
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Fullscreen = SDL_WINDOW_FULLSCREEN,

            /**
             * No window decoration. On iOS this hides the menu bar.
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Borderless = SDL_WINDOW_BORDERLESS,
            #endif

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Allow high DPI.
             *
             * @deprecated Has no effect, as this flag is passed implicitly
             *      on platforms where needed. See
             *      @ref Platform-Sdl2Application-dpi for more information.
             */
            AllowHighDpi CORRADE_DEPRECATED_ENUM("has no effect, passed implicitly on platforms that need it") = 0,
            #endif

            #ifndef CORRADE_TARGET_EMSCRIPTEN
            /**
             * Hidden window
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Hidden = SDL_WINDOW_HIDDEN,

            /**
             * Maximized window
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Maximized = SDL_WINDOW_MAXIMIZED,

            /**
             * Minimized window
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Minimized = SDL_WINDOW_MINIMIZED,

            /**
             * Window with mouse locked
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            MouseLocked = SDL_WINDOW_INPUT_GRABBED,
            #endif

            /**
             * Do not create any GPU context. Use together with
             * @ref Sdl2Application(const Arguments&, const Configuration&),
             * @ref create(const Configuration&) or
             * @ref tryCreate(const Configuration&) to prevent implicit
             * creation of an OpenGL context.
             */
            Contextless = 1u << 31 /* Hope this won't ever conflict with anything */
        };

        /**
         * @brief Window flags
         *
         * @see @ref setWindowFlags()
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<WindowFlag, SDL_WINDOW_RESIZABLE|
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            SDL_WINDOW_FULLSCREEN|SDL_WINDOW_BORDERLESS|SDL_WINDOW_HIDDEN|
            SDL_WINDOW_MAXIMIZED|SDL_WINDOW_MINIMIZED|SDL_WINDOW_INPUT_GRABBED|
            #endif
            Uint32(WindowFlag::Contextless)
            > WindowFlags;
        #else
        typedef Containers::EnumSet<WindowFlag> WindowFlags;
        #endif

        /**
         * @brief DPI scaling policy
         *
         * DPI scaling policy when requesting a particular window size. Can
         * be overriden on command-line using `--magnum-dpi-scaling` or via
         * the `MAGNUM_DPI_SCALING` environment variable.
         * @see @ref setSize(), @ref Platform-Sdl2Application-dpi
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class DpiScalingPolicy: UnsignedByte {
            /**
             * Framebuffer DPI scaling. The window will have the same size as
             * requested, but the framebuffer size will be different. Supported
             * only on macOS and iOS and is also the only supported value
             * there.
             */
            Framebuffer,

            /**
             * Virtual DPI scaling. Scales the window based on UI scaling
             * setting in the system. Falls back to
             * @ref DpiScalingPolicy::Physical on platforms that don't support
             * it. Supported only on desktop platforms (except macOS) and it's
             * the default there.
             *
             * Equivalent to `--magnum-dpi-scaling virtual` passed on
             * command-line.
             */
            Virtual,

            /**
             * Physical DPI scaling. Takes the requested window size as a
             * physical size that a window would have on platform's default DPI
             * and scales it to have the same size on given display physical
             * DPI. On platforms that don't have a concept of a window it
             * causes the framebuffer to match screen pixels 1:1 without any
             * scaling. Supported on desktop platforms except macOS and on
             * mobile and web. Default on mobile and web.
             *
             * Equivalent to `--magnum-dpi-scaling physical` passed on
             * command-line.
             */
            Physical,

            /**
             * Default policy for current platform. Alias to one of
             * @ref DpiScalingPolicy::Framebuffer, @ref DpiScalingPolicy::Virtual
             * or @ref DpiScalingPolicy::Physical depending on platform. See
             * @ref Platform-Sdl2Application-dpi for details.
             */
            Default
        };
        #else
        typedef Implementation::DpiScalingPolicy DpiScalingPolicy;
        #endif

        /*implicit*/ Configuration();
        ~Configuration();

        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        /**
         * @brief Window title
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten"
         *      and @ref CORRADE_TARGET_IOS "iOS".
         */
        std::string title() const { return _title; }
        #endif

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp "Magnum SDL2 Application" @ce.
         * @note In @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" and
         *      @ref CORRADE_TARGET_IOS "iOS" this function does nothing and is
         *      included only for compatibility. You need to set the title
         *      separately in platform-specific configuration file.
         */
        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        Configuration& setTitle(std::string title) {
            _title = std::move(title);
            return *this;
        }
        #else
        template<class T> Configuration& setTitle(const T&) { return *this; }
        #endif

        /** @brief Window size */
        Vector2i size() const { return _size; }

        /**
         * @brief DPI scaling policy
         *
         * If @ref dpiScaling() is non-zero, it has a priority over this value.
         * The `--magnum-dpi-scaling` command-line option has a priority over
         * any application-set value.
         * @see @ref setSize(const Vector2i&, DpiScalingPolicy)
         */
        DpiScalingPolicy dpiScalingPolicy() const { return _dpiScalingPolicy; }

        /**
         * @brief Custom DPI scaling
         *
         * If zero, then @ref dpiScalingPolicy() has a priority over this
         * value. The `--magnum-dpi-scaling` command-line option has a priority
         * over any application-set value.
         * @see @ref setSize(const Vector2i&, const Vector2&)
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /**
         * @brief Set window size
         * @param size              Desired window size
         * @param dpiScalingPolicy  Policy based on which DPI scaling will be set
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp {800, 600} @ce on desktop platforms. On
         * @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" and iOS the default is a
         * zero vector, meaning a value that matches the display or canvas size
         * is autodetected. See @ref Platform-Sdl2Application-dpi for more
         * information.
         * @see @ref setSize(const Vector2i&, const Vector2&)
         */
        Configuration& setSize(const Vector2i& size, DpiScalingPolicy dpiScalingPolicy = DpiScalingPolicy::Default) {
            _size = size;
            _dpiScalingPolicy = dpiScalingPolicy;
            return *this;
        }

        /**
         * @brief Set window size with custom DPI scaling
         * @param size              Desired window size
         * @param dpiScaling        Custom DPI scaling value
         *
         * Compared to @ref setSize(const Vector2i&, DpiScalingPolicy) which
         * autodetects the DPI scaling value according to given policy, this
         * function sets the DPI scaling directly. The resulting
         * @ref Sdl2Application::windowSize() is @cpp size*dpiScaling @ce and
         * @ref Sdl2Application::dpiScaling() is @p dpiScaling.
         */
        Configuration& setSize(const Vector2i& size, const Vector2& dpiScaling) {
            _size = size;
            _dpiScaling = dpiScaling;
            return *this;
        }

        /** @brief Window flags */
        WindowFlags windowFlags() const { return _windowFlags; }

        /**
         * @brief Set window flags
         * @return Reference to self (for method chaining)
         *
         * Default are none.
         */
        Configuration& setWindowFlags(WindowFlags flags) {
            _windowFlags = flags;
            return *this;
        }

        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /** @brief @copybrief GLConfiguration::flags()
         * @deprecated Use @ref GLConfiguration::flags() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::flags() instead") GLConfiguration::Flags flags() const { return _flags; }

        /** @brief @copybrief GLConfiguration::setFlags()
         * @deprecated Use @ref GLConfiguration::setFlags() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::setFlags() instead") Configuration& setFlags(GLConfiguration::Flags flags) {
            _flags = flags;
            return *this;
        }

        /** @brief @copybrief GLConfiguration::version()
         * @deprecated Use @ref GLConfiguration::version() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::version() instead") GL::Version version() const { return _version; }
        #endif

        /** @brief @copybrief GLConfiguration::setVersion()
         * @deprecated Use @ref GLConfiguration::setVersion() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::setVersion() instead") Configuration& setVersion(GL::Version version) {
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            _version = version;
            #else
            static_cast<void>(version);
            #endif
            return *this;
        }

        /** @brief @copybrief GLConfiguration::sampleCount()
         * @deprecated Use @ref GLConfiguration::sampleCount() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::sampleCount() instead") Int sampleCount() const { return _sampleCount; }

        /** @brief @copybrief GLConfiguration::setSampleCount()
         * @deprecated Use @ref GLConfiguration::setSampleCount() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::setSampleCount() instead") Configuration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /** @brief @copybrief GLConfiguration::isSRGBCapable()
         * @deprecated Use @ref GLConfiguration::isSRGBCapable() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::isSRGBCapable() instead") bool isSRGBCapable() const { return _sRGBCapable; }

        /** @brief @copybrief GLConfiguration::setSRGBCapable()
         * @deprecated Use @ref GLConfiguration::setSRGBCapable() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::setSRGBCapable() instead") Configuration& setSRGBCapable(bool enabled) {
            _sRGBCapable = enabled;
            return *this;
        }
        #endif
        #endif

    private:
        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        std::string _title;
        #endif
        Vector2i _size;
        DpiScalingPolicy _dpiScalingPolicy;
        WindowFlags _windowFlags;
        Vector2 _dpiScaling;
        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        Int _sampleCount;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        GL::Version _version;
        Flags _flags;
        bool _sRGBCapable;
        #endif
        #endif
};

/**
@brief Viewport event

@see @ref viewportEvent()
*/
class Sdl2Application::ViewportEvent {
    public:
        /**
         * @brief Window size
         *
         * On some platforms with HiDPI displays, window size can be different
         * from @ref framebufferSize(). See @ref Platform-Sdl2Application-dpi
         * for more information.
         * @see @ref Sdl2Application::windowSize()
         */
        Vector2i windowSize() const { return _windowSize; }

        /**
         * @brief Framebuffer size
         *
         * On some platforms with HiDPI displays, framebuffer size can be
         * different from @ref windowSize(). See
         * @ref Platform-Sdl2Application-dpi for more information.
         * @see @ref Sdl2Application::framebufferSize()
         */
        Vector2i framebufferSize() const { return _framebufferSize; }

        /**
         * @brief DPI scaling
         *
         * On some platforms moving an app between displays can result in DPI
         * scaling value being changed in tandem with a window/framebuffer
         * size. Simply resizing a window doesn't change the DPI scaling value.
         * See @ref Platform-Sdl2Application-dpi for more information.
         * @see @ref Sdl2Application::dpiScaling()
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

    private:
        friend Sdl2Application;

        explicit ViewportEvent(const Vector2i& windowSize, const Vector2i& framebufferSize, const Vector2& dpiScaling): _windowSize{windowSize}, _framebufferSize{framebufferSize}, _dpiScaling{dpiScaling} {}

        Vector2i _windowSize, _framebufferSize;
        Vector2 _dpiScaling;
};

/**
@brief Base for input events

@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref keyReleaseEvent(), @ref mousePressEvent(), @ref mouseReleaseEvent(),
    @ref mouseMoveEvent()
*/
class Sdl2Application::InputEvent {
    public:
        /**
         * @brief Modifier
         *
         * @see @ref Modifiers, @ref KeyEvent::modifiers(),
         *      @ref MouseEvent::modifiers(), @ref MouseMoveEvent::modifiers()
         */
        enum class Modifier: Uint16 {
            /**
             * Shift
             *
             * @see @ref KeyEvent::Key::LeftShift, @ref KeyEvent::Key::RightShift
             */
            Shift = KMOD_SHIFT,

            /**
             * Ctrl
             *
             * @see @ref KeyEvent::Key::LeftCtrl, @ref KeyEvent::Key::RightCtrl
             */
            Ctrl = KMOD_CTRL,

            /**
             * Alt
             *
             * @see @ref KeyEvent::Key::LeftAlt, @ref KeyEvent::Key::RightAlt
             */
            Alt = KMOD_ALT,

            /**
             * Super key (Windows/⌘)
             *
             * @see @ref KeyEvent::Key::LeftSuper, @ref KeyEvent::Key::RightSuper
             */
            Super = KMOD_GUI,

            /**
             * AltGr
             *
             * @see @ref KeyEvent::Key::AltGr
             */
            AltGr = KMOD_MODE,

            CapsLock = KMOD_CAPS,       /**< Caps lock */
            NumLock = KMOD_NUM          /**< Num lock */
        };

        /**
         * @brief Set of modifiers
         *
         * @see @ref KeyEvent::modifiers(), @ref MouseEvent::modifiers(),
         *      @ref MouseMoveEvent::modifiers()
         */
        typedef Containers::EnumSet<Modifier> Modifiers;

        /** @brief Copying is not allowed */
        InputEvent(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent(InputEvent&&) = delete;

        /** @brief Copying is not allowed */
        InputEvent& operator=(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent& operator=(InputEvent&&) = delete;

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        explicit InputEvent(): _accepted(false) {}

        ~InputEvent() = default;
    #endif

    private:
        bool _accepted;
};

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent()
*/
class Sdl2Application::KeyEvent: public Sdl2Application::InputEvent {
    friend Sdl2Application;

    public:
        /**
         * @brief Key
         *
         * @see @ref key()
         */
        enum class Key: SDL_Keycode {
            Unknown = SDLK_UNKNOWN,     /**< Unknown key */

            /**
             * Left Shift
             *
             * @see @ref InputEvent::Modifier::Shift
             */
            LeftShift = SDLK_LSHIFT,

            /**
             * Right Shift
             *
             * @see @ref InputEvent::Modifier::Shift
             */
            RightShift = SDLK_RSHIFT,

            /**
             * Left Ctrl
             *
             * @see @ref InputEvent::Modifier::Ctrl
             */
            LeftCtrl = SDLK_LCTRL,

            /**
             * Right Ctrl
             *
             * @see @ref InputEvent::Modifier::Ctrl
             */
            RightCtrl = SDLK_RCTRL,

            /**
             * Left Alt
             *
             * @see @ref InputEvent::Modifier::Alt
             */
            LeftAlt = SDLK_LALT,

            /**
             * Right Alt
             *
             * @see @ref InputEvent::Modifier::Alt
             */
            RightAlt = SDLK_RALT,

            /**
             * Left Super key (Windows/⌘)
             *
             * @see @ref InputEvent::Modifier::Super
             */
            LeftSuper = SDLK_LGUI,

            /**
             * Right Super key (Windows/⌘)
             *
             * @see @ref InputEvent::Modifier::Super
             */
            RightSuper = SDLK_RGUI,

            /**
             * AltGr
             *
             * @see @ref InputEvent::Modifier::AltGr
             */
            AltGr = SDLK_MODE,

            Enter = SDLK_RETURN,        /**< Enter */
            Esc = SDLK_ESCAPE,          /**< Escape */

            Up = SDLK_UP,               /**< Up arrow */
            Down = SDLK_DOWN,           /**< Down arrow */
            Left = SDLK_LEFT,           /**< Left arrow */
            Right = SDLK_RIGHT,         /**< Right arrow */
            Home = SDLK_HOME,           /**< Home */
            End = SDLK_END,             /**< End */
            PageUp = SDLK_PAGEUP,       /**< Page up */
            PageDown = SDLK_PAGEDOWN,   /**< Page down */
            Backspace = SDLK_BACKSPACE, /**< Backspace */
            Insert = SDLK_INSERT,       /**< Insert */
            Delete = SDLK_DELETE,       /**< Delete */

            F1 = SDLK_F1,               /**< F1 */
            F2 = SDLK_F2,               /**< F2 */
            F3 = SDLK_F3,               /**< F3 */
            F4 = SDLK_F4,               /**< F4 */
            F5 = SDLK_F5,               /**< F5 */
            F6 = SDLK_F6,               /**< F6 */
            F7 = SDLK_F7,               /**< F7 */
            F8 = SDLK_F8,               /**< F8 */
            F9 = SDLK_F9,               /**< F9 */
            F10 = SDLK_F10,             /**< F10 */
            F11 = SDLK_F11,             /**< F11 */
            F12 = SDLK_F12,             /**< F12 */

            Space = SDLK_SPACE,         /**< Space */
            Tab = SDLK_TAB,             /**< Tab */
            Comma = SDLK_COMMA,         /**< Comma */
            Period = SDLK_PERIOD,       /**< Period */
            Minus = SDLK_MINUS,         /**< Minus */
            Plus = SDLK_PLUS,           /**< Plus */
            Slash = SDLK_SLASH,         /**< Slash */
            Percent = SDLK_PERCENT,     /**< Percent */
            Semicolon = SDLK_SEMICOLON, /**< Semicolon */
            Equal = SDLK_EQUALS,        /**< Equal */

            Zero = SDLK_0,              /**< Zero */
            One = SDLK_1,               /**< One */
            Two = SDLK_2,               /**< Two */
            Three = SDLK_3,             /**< Three */
            Four = SDLK_4,              /**< Four */
            Five = SDLK_5,              /**< Five */
            Six = SDLK_6,               /**< Six */
            Seven = SDLK_7,             /**< Seven */
            Eight = SDLK_8,             /**< Eight */
            Nine = SDLK_9,              /**< Nine */

            A = SDLK_a,                 /**< Letter A */
            B = SDLK_b,                 /**< Letter B */
            C = SDLK_c,                 /**< Letter C */
            D = SDLK_d,                 /**< Letter D */
            E = SDLK_e,                 /**< Letter E */
            F = SDLK_f,                 /**< Letter F */
            G = SDLK_g,                 /**< Letter G */
            H = SDLK_h,                 /**< Letter H */
            I = SDLK_i,                 /**< Letter I */
            J = SDLK_j,                 /**< Letter J */
            K = SDLK_k,                 /**< Letter K */
            L = SDLK_l,                 /**< Letter L */
            M = SDLK_m,                 /**< Letter M */
            N = SDLK_n,                 /**< Letter N */
            O = SDLK_o,                 /**< Letter O */
            P = SDLK_p,                 /**< Letter P */
            Q = SDLK_q,                 /**< Letter Q */
            R = SDLK_r,                 /**< Letter R */
            S = SDLK_s,                 /**< Letter S */
            T = SDLK_t,                 /**< Letter T */
            U = SDLK_u,                 /**< Letter U */
            V = SDLK_v,                 /**< Letter V */
            W = SDLK_w,                 /**< Letter W */
            X = SDLK_x,                 /**< Letter X */
            Y = SDLK_y,                 /**< Letter Y */
            Z = SDLK_z                  /**< Letter Z */
        };

        /**
         * @brief Name for given key
         *
         * Human-readable localized UTF-8 name for given @p key, intended for
         * displaying to the user in e.g. key binding configuration. If there
         * is no name for given key, empty string is returned.
         * @see @ref keyName(Key)
         */
        static std::string keyName(Key key);

        /**
         * @brief Key
         *
         * @see @ref keyName()
         */
        Key key() const { return _key; }

        /**
         * @brief Key name
         *
         * Human-readable localized UTF-8 name for the key returned by
         * @ref key(), intended for displaying to the user in e.g.
         * key binding configuration. If there is no name for that key, empty
         * string is returned.
         * @see @ref keyName(Key)
         */
        std::string keyName() const;

        /** @brief Modifiers */
        Modifiers modifiers() const { return _modifiers; }

        /**
         * @brief Whether the key press is repeated
         *
         * Returns @cpp true @ce if the key press event is repeated,
         * @cpp false @ce if not or if this was key release event.
         */
        bool isRepeated() const { return _repeated; }

    private:
        explicit KeyEvent(Key key, Modifiers modifiers, bool repeated): _key{key}, _modifiers{modifiers}, _repeated{repeated} {}

        const Key _key;
        const Modifiers _modifiers;
        const bool _repeated;
};

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref MouseScrollEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent()
*/
class Sdl2Application::MouseEvent: public Sdl2Application::InputEvent {
    friend Sdl2Application;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: Uint8 {
            Left = SDL_BUTTON_LEFT,         /**< Left button */
            Middle = SDL_BUTTON_MIDDLE,     /**< Middle button */
            Right = SDL_BUTTON_RIGHT,       /**< Right button */

            /** First extra button (e.g. wheel left) */
            X1 = SDL_BUTTON_X1,

            /** Second extra button (e.g. wheel right) */
            X2 = SDL_BUTTON_X2,

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Wheel up
             * @deprecated Use @ref MouseScrollEvent and @ref mouseScrollEvent() instead.
             */
            WheelUp CORRADE_DEPRECATED_ENUM("use mouseScrollEvent() and MouseScrollEvent instead") = SDL_BUTTON_X2 + 1,

            /**
             * Wheel down
             * @deprecated Use @ref MouseScrollEvent and @ref mouseScrollEvent() instead.
             */
            WheelDown CORRADE_DEPRECATED_ENUM("use mouseScrollEvent() and MouseScrollEvent instead") = SDL_BUTTON_X2 + 2
            #endif
        };

        /** @brief Button */
        Button button() const { return _button; }

        /** @brief Position */
        Vector2i position() const { return _position; }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Click count
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Int clickCount() const { return _clickCount; }
        #endif

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        explicit MouseEvent(Button button, const Vector2i& position
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            , Int clickCount
            #endif
            ): _button{button}, _position{position},
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            _clickCount{clickCount},
            #endif
            _modifiersLoaded{false} {}

        const Button _button;
        const Vector2i _position;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        const Int _clickCount;
        #endif
        bool _modifiersLoaded;
        Modifiers _modifiers;
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref MouseScrollEvent, @ref mouseMoveEvent()
*/
class Sdl2Application::MouseMoveEvent: public Sdl2Application::InputEvent {
    friend Sdl2Application;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref Buttons, @ref buttons()
         */
        enum class Button: Uint32 {
            Left = SDL_BUTTON_LMASK,        /**< Left button */
            Middle = SDL_BUTTON_MMASK,      /**< Middle button */
            Right = SDL_BUTTON_RMASK,       /**< Right button */

            /** First extra button (e.g. wheel left) */
            X1 = SDL_BUTTON_X1MASK,

            /** Second extra button (e.g. wheel right) */
            X2 = SDL_BUTTON_X2MASK
        };

        /**
         * @brief Set of mouse buttons
         *
         * @see @ref buttons()
         */
        typedef Containers::EnumSet<Button> Buttons;

        /** @brief Position */
        Vector2i position() const { return _position; }

        /**
         * @brief Relative position
         *
         * Position relative to previous event.
         */
        Vector2i relativePosition() const { return _relativePosition; }

        /** @brief Mouse buttons */
        Buttons buttons() const { return _buttons; }

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        explicit MouseMoveEvent(const Vector2i& position, const Vector2i& relativePosition, Buttons buttons): _position{position}, _relativePosition{relativePosition}, _buttons{buttons}, _modifiersLoaded{false} {}

        const Vector2i _position, _relativePosition;
        const Buttons _buttons;
        bool _modifiersLoaded;
        Modifiers _modifiers;
};

/**
@brief Mouse scroll event

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class Sdl2Application::MouseScrollEvent: public Sdl2Application::InputEvent {
    friend Sdl2Application;

    public:
        /** @brief Scroll offset */
        Vector2 offset() const { return _offset; }

        /**
         * @brief Position
         *
         * Lazily populated on first request.
         */
        Vector2i position();

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        explicit MouseScrollEvent(const Vector2& offset): _offset{offset}, _positionLoaded{false}, _modifiersLoaded{false} {}

        const Vector2 _offset;
        bool _positionLoaded;
        bool _modifiersLoaded;
        Vector2i _position;
        Modifiers _modifiers;
};

/**
@brief Multi gesture event

@experimental
@see @ref multiGestureEvent()
*/
class Sdl2Application::MultiGestureEvent {
    friend Sdl2Application;

    public:
        /** @brief Copying is not allowed */
        MultiGestureEvent(const MultiGestureEvent&) = delete;

        /** @brief Moving is not allowed */
        MultiGestureEvent(MultiGestureEvent&&) = delete;

        /** @brief Copying is not allowed */
        MultiGestureEvent& operator=(const MultiGestureEvent&) = delete;

        /** @brief Moving is not allowed */
        MultiGestureEvent& operator=(MultiGestureEvent&&) = delete;

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Gesture center */
        Vector2 center() const { return _center; }

        /**
         * @brief Relative rotation
         *
         * Rotation relative to previous event.
         */
        Float relativeRotation() const { return _relativeRotation; }

        /**
         * @brief Relative distance
         *
         * Distance of the fingers relative to previous event.
         */
        Float relativeDistance() const { return _relativeDistance; }

        /**
         * @brief Finger count
         *
         * Count of fingers performing the gesture.
         */
        Int fingerCount() const { return _fingerCount; }

    private:
        explicit MultiGestureEvent(const Vector2& center, Float relativeRotation, Float relativeDistance, Int fingerCount): _center{center}, _relativeRotation{relativeRotation}, _relativeDistance{relativeDistance}, _fingerCount{fingerCount}, _accepted{false} {}

        Vector2 _center;
        Float _relativeRotation,
            _relativeDistance;
        Int _fingerCount;
        bool _accepted;
};

/**
@brief Text input event

@see @ref TextEditingEvent, @ref textInputEvent()
*/
class Sdl2Application::TextInputEvent {
    friend Sdl2Application;

    public:
        /** @brief Copying is not allowed */
        TextInputEvent(const TextInputEvent&) = delete;

        /** @brief Moving is not allowed */
        TextInputEvent(TextInputEvent&&) = delete;

        /** @brief Copying is not allowed */
        TextInputEvent& operator=(const TextInputEvent&) = delete;

        /** @brief Moving is not allowed */
        TextInputEvent& operator=(TextInputEvent&&) = delete;

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Input text in UTF-8 */
        Containers::ArrayView<const char> text() const { return _text; }

    private:
        explicit TextInputEvent(Containers::ArrayView<const char> text): _text{text}, _accepted{false} {}

        Containers::ArrayView<const char> _text;
        bool _accepted;
};

/**
@brief Text editing event

@see @ref textEditingEvent()
*/
class Sdl2Application::TextEditingEvent {
    friend Sdl2Application;

    public:
        /** @brief Copying is not allowed */
        TextEditingEvent(const TextEditingEvent&) = delete;

        /** @brief Moving is not allowed */
        TextEditingEvent(TextEditingEvent&&) = delete;

        /** @brief Copying is not allowed */
        TextEditingEvent& operator=(const TextEditingEvent&) = delete;

        /** @brief Moving is not allowed */
        TextEditingEvent& operator=(TextEditingEvent&&) = delete;

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Input text in UTF-8 */
        Containers::ArrayView<const char> text() const { return _text; }

        /** @brief Location to begin editing from */
        Int start() const { return _start; }

        /** @brief Number of characters to edit from the start point */
        Int length() const { return _length; }

    private:
        explicit TextEditingEvent(Containers::ArrayView<const char> text, Int start, Int length): _text{text}, _start{start}, _length{length}, _accepted{false} {}

        Containers::ArrayView<const char> _text;
        Int _start, _length;
        bool _accepted;
};

/** @hideinitializer
@brief Entry point for SDL2-based applications
@param className Class name

See @ref Magnum::Platform::Sdl2Application "Platform::Sdl2Application" for
usage information. This macro abstracts out platform-specific entry point code
and is equivalent to the following on all supported platforms except
@ref CORRADE_TARGET_WINDOWS_RT "Windows RT", see @ref portability-applications
for more information.

@code{.cpp}
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode

When no other application header is included this macro is also aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce.
*/
#ifndef CORRADE_TARGET_WINDOWS_RT
#define MAGNUM_SDL2APPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }
#else
#define MAGNUM_SDL2APPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }                                                                       \
    __pragma(warning(push))                                                 \
    __pragma(warning(disable: 4447))                                        \
    int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {                \
        if(FAILED(Windows::Foundation::Initialize(RO_INIT_MULTITHREADED)))  \
            return 1;                                                       \
        return SDL_WinRTRunApp(main, nullptr);                              \
    }                                                                       \
    __pragma(warning(pop))
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef Sdl2Application Application;
typedef BasicScreen<Sdl2Application> Screen;
typedef BasicScreenedApplication<Sdl2Application> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_SDL2APPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Flags)
CORRADE_ENUMSET_OPERATORS(Sdl2Application::Configuration::WindowFlags)
CORRADE_ENUMSET_OPERATORS(Sdl2Application::InputEvent::Modifiers)
CORRADE_ENUMSET_OPERATORS(Sdl2Application::MouseMoveEvent::Buttons)

}}

#endif
