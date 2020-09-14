import sys
import argparse

import construct as c
import click


FIRMWARE_HEADER_STRUCT = c.Struct(
    'signature' / c.Const(b"CSYS"),
    c.Const(b'\x00\x00\xC0\x00'),
    c.Const(b'\x00\x00\x05\x00'),
    'model_id' / c.Bytes(4),
    'image_size' / c.Int32ul,
)

WORD_SIZE = 2
WORD_MASK = 0xFFFF

HEADER_SIZE = 20
CHECKSUM_SIZE = WORD_SIZE

def caclulate_checksum(firmware_path):
    print(f"Calculating checksum on firmware: {firmware_path}")
    with open(firmware_path, 'rb') as firmware:
        header = FIRMWARE_HEADER_STRUCT.parse(firmware.read(HEADER_SIZE))
        print(f"Header struct: \n{header}")

        words_to_read = int((header.image_size - CHECKSUM_SIZE) / WORD_SIZE)
        image_sum = 0
        for _ in range(0, words_to_read):
            current_word = firmware.read(WORD_SIZE)
            image_sum += int.from_bytes(current_word, 'little')


    checksum = -image_sum & WORD_MASK
    checksum_offset = HEADER_SIZE + header.image_size - CHECKSUM_SIZE

    return checksum, checksum_offset

def parse_args():
    parser = argparse.ArgumentParser(description="Recaluclates firmware checksum for edimax routers (specificaly EDIMAX BR-6524n)")

    parser.add_argument('path', help="Path of firmware file.", type=str)

    return parser.parse_args()


def main():
    args = parse_args()
    firmware_path = args.path

    checksum, checksum_offset = caclulate_checksum(firmware_path)
    print(f"Firmware {firmware_path} checksum: {hex(checksum)}")


    if click.confirm("Do you want to update the firmware checksum?"):
        with open(firmware_path, 'r+b') as firmware:
            firmware.seek(checksum_offset)
            firmware.write(checksum.to_bytes(CHECKSUM_SIZE, 'little'))


if __name__ == "__main__":
        main()