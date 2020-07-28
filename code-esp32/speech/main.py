from pydub import AudioSegment
# from playsound import playsound
import os
import simpleaudio as sa
from ffmpeg import audio

def cut_audio(in_filename, out_filename, start, lenth):
    sound = AudioSegment.from_file(in_filename)
    stop = start + lenth
    print("time:", start, "~", stop)
    # start_time = (int(start_time.split(':')[0])*60+int(start_time.split(':')[1]))*1000
    # stop_time = (int(stop_time.split(':')[0])*60+int(stop_time.split(':')[1]))*1000
    # print("ms:",start_time,"~",stop_time)
    word = sound[start:stop]
    save_name = out_filename
    word.export(save_name, format="wav")

def play(filename):
    wave_obj = sa.WaveObject.from_wave_file(filename)
    play_obj = wave_obj.play()
    play_obj.wait_done()

def add_num(sound, num):
    if num > 9:
        print('num err')
        return
    print("add num %d" % num)
    name = 'voice/' + str(num) + '.wav'
    sound.append(AudioSegment.from_file(name))

def add_char(sound, char):
    print("add char %s" % char)
    name = 'voice/' + char + '.wav'
    sound.append(AudioSegment.from_file(name))

def add_decimal(sound, index):
    char = 'ge'
    if index == 1:
        return
    if index == 2:
        char = 'shi'
    elif index == 3:
        char = 'bai'
    elif index == 4:
        char = 'qian'
    add_char(sound, char)

def add_unit(sound, unit):
    add_char(sound, unit)


def add_integral(sound, num):
    s = str(num)
    s = s[:4]

    i = 0
    last_zero = -1
    while i < len(s):
        num = int(s[i])

        if last_zero == -1 or num != 0:
            add_num(sound, num)
        if num != 0:
            last_zero = -1
            add_decimal(sound, len(s) - i)
        else:
            last_zero = i

        # check is all zero
        zero_num = 0
        for j in range(i + 1, len(s)):
            if int(s[j]) == 0:
                zero_num = zero_num + 1
        if zero_num == (len(s) - i - 1):
            break
        i = i + 1

def synthesis(num, unit1, unit2=0):
    sound = []

    if num < 1000:
        s = str(num)
        s = s[:5]
        Integral = s.split('.')[0]
        Fractional = s.split('.')[1]

        add_integral(sound, int(Integral))

        add_char(sound, 'dian')

        for i in range(0, len(Fractional)):
            add_num(sound, int(Fractional[i]))
    else:
        add_integral(sound, num)

    add_unit(sound, unit1)
    if unit2:
        add_unit(sound, unit2)

    out = AudioSegment.empty()
    for i in range(0, len(sound)):
        out = out + sound[i]

    out.export("out.wav", format="wav")
    play('out.wav')



def get_audio_info(filename):
    sound = AudioSegment.from_file(filename)
    print('channels=%d, frame_rate=%d, frame_width=%d, time=%d' % (sound.channels, sound.frame_rate, sound.frame_width, len(sound)))

def generate_audio(path, acc=1):

    for rt, dirs, files in os.walk(path):
        for f in files:
            if dirs:
                continue
            print(f)
            file_path = path + f
            fname = os.path.splitext(f)[0]
            if acc != 1:
                path_acc = path + 'audio_file_acc/'
                try:
                    os.mkdir(path_acc, 777)
                except Exception:
                   pass
                audio.a_speed(file_path, acc, path_acc+f)
                file_path = path_acc + f

            cut_audio(file_path, "voice/%s.wav" % fname, 0, 230)


if __name__ == "__main__":
    # get_audio_info('../../3.mp3')
    generate_audio('audio_file/', acc=1.6)

    # synthesis(1000)
    #
    # synthesis(1200)
    # synthesis(1020)
    # synthesis(1002)
    #
    # synthesis(1230)
    # synthesis(1203)
    synthesis(1023, 'hao', 'fu')
    #
    synthesis(1.268, 'wei', 'an')

    # synthesis(130.3, 'fu')

