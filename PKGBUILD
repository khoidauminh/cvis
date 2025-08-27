pkgname='cvis'
pkgver=0.1.0
pkgrel=1
pkgdesc='Tiny Music Visualizer - C port'
arch=('x86_64')
source=()
depends=('sdl3' 'ncurses')
makedepends=('meson')

build() {
    cd ".."
    arch-meson build
    meson compile -C build
}

check() {
    cd ".."
    meson test -C build --print-errorlogs
}

package() {
    cd ".."
    meson install -C build --destdir "$pkgdir"
}
