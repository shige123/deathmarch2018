version = 4
prev_block = "0000000000000000005629ef6b683f8f6301c7e6f8e796e7c58702a079db14e8"
markle_root = "efb8011cb97b5f1599b2e18f200188f1b8207da2884392672f92ac7985534eeb"
timestamp = "2016-01-30 13:23:09"
bits = 403253488
#nonce = 1448681410
nonce = 1440181011

print type(version)
version_h = format(version, "08x").decode("hex")[::-1]
print(type(version_h))
print version_h


prev_block_h = prev_block.decode("hex")[::-1]
markle_root_h = markle_root.decode("hex")[::-1]
print prev_block_h

from datetime import datetime
timestamp_s = int((datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S")-datetime(1970,1,1)).total_seconds())
timestamp_h = format(timestamp_s,"x").decode("hex")[::-1]
print timestamp_s
print timestamp_h

bits_h = format(bits,"x").decode("hex")[::-1]
nonce_h = format(nonce,"x").decode("hex")[::-1]
print bits_h
print nonce_h

from hashlib import sha256
print (sha256("0000000000000000005629ef6b683f8f6301c7e6f8e796e7c58702a079db14e8").digest()[::-1].encode("hex"))
header = version_h + prev_block_h + markle_root_h + timestamp_h + bits_h + nonce_h
print sha256(header).digest()[::-1].encode("hex")
print(sha256(sha256(header).digest()).digest()[::-1].encode("hex"))
