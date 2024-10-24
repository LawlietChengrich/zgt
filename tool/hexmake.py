'''
----------------------------------------------------------------------------------------------------

									hexmake.py
									bootloader hex + app hex
									= all hex

----------------------------------------------------------------------------------------------------
'''
ADDRESS_BOOTLOADER = '08000000'
ADDRESS_BOOTLOADER_INF = '08008000'
ADDRESS_MAIN = '08020000'
ADDRESS_BACKUP_0 = '08040000'
ADDRESS_CHECKSUM_0 = '0805FC00'
ADDRESS_BACKUP_1 = '08060000'
ADDRESS_CHECKSUM_1 = '0807FC00'
ADDRESS_BACKUP_2 = '08080000'
ADDRESS_CHECKSUM_2 = '0809FC00'

KEYWORD_MAIN = ':04000005'
KEYWORD_END = ':00000001FF'
KEYWORD_ADDR = ':02000004'
MAX_LINE_DATA_LEN = 16
POSITION_DATA = 9
POSITION_DATA_TYPE = 7
POSITION_DATA_LEN = 1
POSITION_SUB_ADDRESS = 3
CHAR_NULL = 'FF'
CMD_DATA_REC = '00'

class Settings:
	# 1. Input .hex file
	inputFile = r'..\bootloader\MDK-ARM\g60_fireboard_bl\g60_fireboard_bl.hex'
	inputFile1 = r'..\app\MDK-ARM\stm32_zg_test\stm32_app_build.hex'

	# 2. Output .c file
	outputFile = [r'..\project_all.hex']

	# 3.
	programName    = 'project_all'

class Convertor:
	def __init__(self):
		self.set = Settings()
		self.bootloader_flash_src_len = 0
		self.bootloader_flash_checksum = 0
		self.app_flash_src_len = 0
		self.app_flash_checksum = 0
		self.hex_file_len = 0
	
	def endian_change(self, value):
		change_value = 0
		change_value |= (value & 0xff) << 24
		change_value |= (value & 0xff00) << 8
		change_value |= (value & 0xff0000) >> 8
		change_value |= (value & 0xff000000) >>24

		return change_value

	def readHexFile(self, file_bootloader, file_app):
		retArray = []

		try:
			fin = open(file_bootloader, 'r')
			for line in fin:
				if KEYWORD_MAIN in line or KEYWORD_END in line:
					#print(line)
					continue

				line_type = int(line[POSITION_DATA_TYPE:(POSITION_DATA_TYPE+2)], 16)
				#print(type(line))
				if line_type == 0:
					line_data_len = int(line[POSITION_DATA_LEN:(POSITION_DATA_LEN+2)],16)
					self.bootloader_flash_src_len += line_data_len
					for i in range(line_data_len):
						self.bootloader_flash_checksum += int(line[(POSITION_DATA+2*i):(POSITION_DATA+2*(i+1))],16)

					if line_data_len < MAX_LINE_DATA_LEN:
						insert_posi = POSITION_DATA + 2*line_data_len
						insert_num = MAX_LINE_DATA_LEN - line_data_len
						for i in range(insert_num):
							line = line[0:insert_posi] + CHAR_NULL + line[insert_posi:]
						line = ''+':%02x'%MAX_LINE_DATA_LEN + line[POSITION_SUB_ADDRESS:]

				retArray.append(line)
				self.hex_file_len += len(line)

			self.bootloader_flash_checksum = self.bootloader_flash_checksum & 0xffffffff
		except:
			print('Can not open the \"{}\" file!'.format(file_bootloader))
			import sys
			sys.exit(0)

		try:
			address_par = [ADDRESS_MAIN[0:4], ADDRESS_BACKUP_0[0:4], ADDRESS_BACKUP_1[0:4], ADDRESS_BACKUP_2[0:4]]
			address_checksum_par = [ADDRESS_CHECKSUM_0, ADDRESS_CHECKSUM_1, ADDRESS_CHECKSUM_2]
			app_data_array = []
			for x in range(4):
				fin = open(file_app, 'r')
				for line in fin:
					if KEYWORD_MAIN in line or KEYWORD_END in line:
						#print(line)
						continue

					if KEYWORD_ADDR in line:
						line_checksum = 0
						line = KEYWORD_ADDR + address_par[x]
						for i in range(int((len(line)-1)/2)):
							line_checksum += int(line[POSITION_DATA_LEN+2*i:POSITION_DATA_LEN+2*(i+1)],16)

						line_checksum = 0x100 - (line_checksum&0xff)
						line_checksum &= 0xff

						line = line  + '%02X'% line_checksum + '\n'
						#line = line[0:POSITION_DATA] + address_par[x] + '\n'

					#line_len = int(line[POSITION_DATA_LEN:(POSITION_DATA_LEN+2)],16)
					line_type = int(line[POSITION_DATA_TYPE:(POSITION_DATA_TYPE+2)], 16)
					#print(type(line))
					if line_type == 0:
						line_data_len = int(line[POSITION_DATA_LEN:(POSITION_DATA_LEN+2)],16)
						if x == 0:
							#line_data_len = int(line[POSITION_DATA_LEN:(POSITION_DATA_LEN+2)],16)
							self.app_flash_src_len += line_data_len
							for i in range(line_data_len):
								self.app_flash_checksum += int(line[(POSITION_DATA+2*i):(POSITION_DATA+2*(i+1))],16)

						if line_data_len < MAX_LINE_DATA_LEN:
							insert_posi = POSITION_DATA + 2*line_data_len
							insert_num = MAX_LINE_DATA_LEN - line_data_len
							for i in range(insert_num):
								line = line[0:insert_posi] + CHAR_NULL + line[insert_posi:]
							line = ''+':%02x'%MAX_LINE_DATA_LEN + line[POSITION_SUB_ADDRESS:]

					#retArray.append(line)
					app_data_array.append(line)
					self.hex_file_len += len(line)

				if x > 0:
					checksum_line = KEYWORD_ADDR + address_checksum_par[x-1][0:4]
					line_checksum = 0

					for i in range(int((len(checksum_line)-1)/2)):
						line_checksum += int(checksum_line[POSITION_DATA_LEN+2*i:POSITION_DATA_LEN+2*(i+1)],16)

					line_checksum = 0x100 - (line_checksum&0xff)
					line_checksum &= 0xff

					checksum_line = checksum_line  + '%02X'% line_checksum + '\n'
					app_data_array.append(checksum_line)
					
					line_checksum = 0
					checksum_line = ':%02x'%MAX_LINE_DATA_LEN + address_checksum_par[x-1][4:8] + CMD_DATA_REC + \
										'%08X'% self.app_flash_src_len + '%08X'% self.app_flash_checksum + \
										'FFFFFFFFFFFFFFFF'
					for i in range(int((len(checksum_line)-1)/2)):
						line_checksum += int(checksum_line[POSITION_DATA_LEN+2*i:POSITION_DATA_LEN+2*(i+1)],16)

					line_checksum = 0x100 - (line_checksum&0xff)
					line_checksum &= 0xff

					checksum_line = checksum_line + '%02X'% line_checksum + '\n'
					app_data_array.append(checksum_line)
				else:
					self.app_flash_checksum = self.app_flash_checksum & 0xffffffff
					self.app_flash_src_len = self.endian_change(self.app_flash_src_len)
					self.app_flash_checksum = self.endian_change(self.app_flash_checksum)

		except:
			print('Can not open the \"{}\" file!'.format(file_app))
			import sys
			sys.exit(0)

		flash_src_inf = KEYWORD_ADDR + ADDRESS_BOOTLOADER_INF[0:4]
		line_checksum = 0

		for i in range(int((len(flash_src_inf)-1)/2)):
			line_checksum += int(flash_src_inf[POSITION_DATA_LEN+2*i:POSITION_DATA_LEN+2*(i+1)],16)

		line_checksum = 0x100 - (line_checksum&0xff)
		line_checksum &= 0xff

		flash_src_inf = flash_src_inf  + '%02X'% line_checksum + '\n'

		#bootloader size , 4字节， bootloader 校验和，4字节

		self.bootloader_flash_src_len = self.endian_change(self.bootloader_flash_src_len)
		self.bootloader_flash_checksum = self.endian_change(self.bootloader_flash_checksum)

		line_checksum = 0
		line_checksum_str = ':%02x'%MAX_LINE_DATA_LEN + ADDRESS_BOOTLOADER_INF[4:8] + CMD_DATA_REC + \
							'%08X'% self.bootloader_flash_src_len + '%08X'% self.bootloader_flash_checksum + \
							'FFFFFFFFFFFFFFFF'
		for i in range(int((len(line_checksum_str)-1)/2)):
			line_checksum += int(line_checksum_str[POSITION_DATA_LEN+2*i:POSITION_DATA_LEN+2*(i+1)],16)

		line_checksum = 0x100 - (line_checksum&0xff)
		line_checksum &= 0xff

		line_checksum_str = line_checksum_str + '%02X'% line_checksum + '\n'
		flash_src_inf = flash_src_inf + line_checksum_str

		retArray.append(flash_src_inf)
		retArray = retArray + app_data_array
		retArray.append(KEYWORD_END)
		self.hex_file_len += len(KEYWORD_END)
		#print(self.hex_file_len)
		#print(self.app_flash_src_len)
		#print(self.bootloader_flash_src_len)
		return retArray

	def buildCFile(self, file, bArray):
		with open(file, 'w') as fout:
			s = ''.join(bArray)
			fout.write(s)

	def outputInfo(self):
		self.bootloader_flash_src_len = self.endian_change(self.bootloader_flash_src_len)
		self.bootloader_flash_checksum = self.endian_change(self.bootloader_flash_checksum)
		self.app_flash_src_len = self.endian_change(self.app_flash_src_len)
		self.app_flash_checksum = self.endian_change(self.app_flash_checksum)
		print(' ')
		print('=============================== hex_all make ===============================')
		print(' ')
		print('bootloader  flash address: 0x' + ADDRESS_BOOTLOADER)
		print('            flash size:    ' + str(self.bootloader_flash_src_len) + ' bytes')
		print('app main flash address:    0x' + ADDRESS_MAIN)
		print('            flash size:    ' + str(int(self.app_flash_src_len)) + ' bytes')
		print('app backup0 flash address: 0x' + ADDRESS_BACKUP_0)
		print('            flash size:    ' + str(int(self.app_flash_src_len)) + ' bytes')
		print('app backup1 flash address: 0x' + ADDRESS_BACKUP_1)
		print('            flash size:    ' + str(int(self.app_flash_src_len)) + ' bytes')
		print('app backup2 flash address: 0x' + ADDRESS_BACKUP_2)
		print('            flash size:    ' + str(int(self.app_flash_src_len)) + ' bytes')
		print('hex file size: ' + str(self.hex_file_len) + ' bytes')
		print('=============================== hex_all make ===============================')
		print(' ')

	def changePath(self):
		import os
		abs_file = os.path.abspath(__file__)
		abs_dir = abs_file[:abs_file.rfind('\\')]
		os.chdir(abs_dir)

	def do(self):
		self.changePath()
		binArraySrc = self.readHexFile(self.set.inputFile, self.set.inputFile1)
		self.buildCFile(self.set.outputFile[0], binArraySrc)
		self.outputInfo()


Convertor().do()

