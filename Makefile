all:
	make -C src/g3d
	make -C src/mcubes

clean:
	make -C src/g3d clean
	make -C src/mcubes clean
