pkgname='cvis'
pkgver=0.1.0
pkgrel=1
pkgdesc='Tiny Music Visualizer - C port'
arch=('x86_64')
source=()
depends=('sdl3' 'ncurses')
makedepends=('meson')

build() {
    arch-meson "$startdir" build
    meson compile -C build
}

check() {
    meson test -C build --print-errorlogs
}

package() {
    # cd "$pkgname"
    meson install -C build --destdir "$pkgdir"
}
