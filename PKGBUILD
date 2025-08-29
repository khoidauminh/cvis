pkgname='cvis'
pkgver=0.1.0
pkgrel=1
pkgdesc='Tiny Music Vissualizer - C port'
arch=('x86_64')
source=()
depends=('sdl3' 'ncurses')
makedepends=('meson')

build() {
    arch-meson build
    meson compile -C build
}

check() {
    meson test -C build --print-errorlogs
}

package() {
    meson install -C build --destdir "$pkgdir"
}
