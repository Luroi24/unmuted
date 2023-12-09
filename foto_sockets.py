import io
import socket
import time
import numpy as np
from PIL import Image

HOST = "0.0.0.0"
PORT =  1234


serv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# address '0.0.0.0' or '' work to allow connections from other machines.  'localhost' disallows external connections.
# see https://www.raspberrypi.org/forums/viewtopic.php?t=62108
serv.bind((HOST, PORT))
serv.listen(5)
print("Ready to accept 5 connections")


def create_image_from_bytes(image_bytes) -> Image.Image:
    stream = io.BytesIO(image_bytes)
    return Image.open(stream)


while True:
    conn, addr = serv.accept()
    array_from_client = bytearray()
    shape = None
    data = 0
    chunks_received = 0
    start = time.time()
    shape_string = ''
    while True:
        # print('waiting for data')
        # Try 4096 if unsure what buffer size to use. Large transfer chunk sizes (which require large buffers) can cause corrupted results
        data = conn.recv(4096)
        if not data or data == b'tx_complete':
            break
        elif shape is None:
            shape_string += data.decode("utf-8")
            # Find the end of the line.  An index other than -1 will be returned if the end has been found because 
            # it has been received
            if shape_string.find('\r\n') != -1:
                width_index = shape_string.find('width:')
                height_index = shape_string.find('height:')
                width = int(shape_string[width_index + len('width:'): height_index])
                height = int(shape_string[height_index + len('height:'): ])
                shape = (width, height)
            print("shape is {}".format(shape))
        else:
            chunks_received += 1
            # print(chunks_received)
            array_from_client.extend(data)
            # print(array_from_client)
        #conn.sendall(b'8')
        # print("sent acknowledgement")
    #     TODO: need to check if sending acknowledgement of the number of chunks and the total length of the array is a good idea
    print("chunks_received {}. Number of bytes {}".format(chunks_received, len(array_from_client)))
    img: Image.Image = create_image_from_bytes(array_from_client)
    img.save("test.png")
    array_start_time = time.time()
    image_array = np.asarray(img)
    print('array conversion took {} s'.format(time.time() - array_start_time))
    conn.close()
    print('client disconnected')