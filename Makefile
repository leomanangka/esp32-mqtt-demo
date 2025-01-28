all:
	pio -f -c vim run -v

upload:
	pio -f -c vim run -v --target upload

clean:
	pio -f -c vim run -v --target clean

program:
	pio -f -c vim run -v --target program

uploadfs:
	pio -f -c vim run -v --target uploadfs

update:
	pio pkg update

compiledb:
	pio -f -c vim run --target compiledb

menuconfig:
	pio -f -c vim run --target menuconfig

erase:
	pio -f -c vim run --target erase -v

