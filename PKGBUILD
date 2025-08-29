pkgname='cvis'
pkgver=0.1.0
pkgrel=1
pkgdesc='Tiny Music Vissualizer - C port'
arch=('x86_64')
source=('git+https://github.com/khoidauminh/cvis')
depends=('sdl3' 'ncurses')
makedepends=('meson')
b2sum=('SKIP')

build() {
    meson setup --prefix=/usr --buildtype=plain "$pkgver" build
    meson compile -C build
}

package() {
    cd ".."
    meson install -C build --destdir "$pkgdir"
}
