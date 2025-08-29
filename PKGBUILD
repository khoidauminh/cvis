pkgname='cvis'
pkgver=0.1.0
pkgrel=1
pkgdesc='Tiny Music Vissualizer - C port'
arch=('x86_64')
source=('git+https://github.com/khoidauminh/cvis')
depends=('sdl3' 'ncurses' 'miniaudio-git')
makedepends=('meson')
b2sums=('SKIP')

build() {
    arch-meson "$pkgname" build
    meson compile -C build
}

package() {
    cd "$pkgname"
    meson install -C build --destdir "$pkgdir"
}
