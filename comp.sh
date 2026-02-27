    if [ -f bin/g-- ]; then
        rm bin/g--
    fi
    cd build/
    cmake ../
    cmake --build .
    cd ../bin/
    if [ -f g-- ]; then
        ./g--
    fi
    cd ../
