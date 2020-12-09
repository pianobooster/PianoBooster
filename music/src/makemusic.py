#!/usr/bin/env python3

# Python script to build the music zip file
# ensure that `po4a po4a-booster-music.cfg` is run first which generates the translations
from __future__ import absolute_import
from __future__ import print_function
import os
import shutil
import sys

info_pages = "InfoPages"
music_release = "4"
build_dir = "build/"
temp_dir = build_dir + "temp/"
zip_file_name = "BoosterMusicBooks"
build_zip_dir = build_dir + zip_file_name + music_release + '/'
build_web_dir = build_dir + "MusicWeb/"
build_translations_dir = build_dir + "translations/"


def find_files_in_dir(dir_name, extension):
    song_names = []
    ext_len = len(extension)
    for file in os.listdir(dir_name):
        if file.endswith(extension):
            song_names.append(file[:-ext_len])
    song_names.sort()
    return song_names


def make_dir(output_dir):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)


def rm_tree(tree_dir):
    if os.path.isdir(tree_dir):
        shutil.rmtree(tree_dir)


def rm_file(file_name):
    if os.path.isfile(file_name):
        os.remove(file_name)


def execute_command(cmd):
    print(cmd)
    if os.system(cmd) != 0:
        sys.exit("Error in: " + cmd)


def create_midi_files(src_dir, book_name, song, temp_mma_dir):
    src_path_name = src_dir + '/' + song
    execute_command("abc2midi \"{0}.abc\" -o \"{1}.solo.mid\" -RS".format(src_path_name, temp_mma_dir + song))
    os.system("cp \"{0}.mma\" \"{1}\"".format(src_path_name, temp_mma_dir))
    execute_command("cd \"{0}\";mma  \"{1}.mma\" -f \"{2}.mid\"".format(temp_mma_dir, song, song))
    os.system("cp \"{0}.mid\" \"{1}\"".format(temp_mma_dir + song, build_zip_dir + book_name))


def create_info_pages(src_dir, book_name, song):
    src_path_name = src_dir + '/' + song
    info_output_dir = build_zip_dir + book_name + '/' + info_pages
    execute_command("pandoc -r markdown -w html \"{0}.md\" -o \"{1}/en/{2}.html\""
                    .format(src_path_name, info_output_dir, song))

    translations_dir = build_translations_dir + src_dir
    if not os.path.isdir(translations_dir):
        sys.exit("Error no languages in '{0}'\nEnsure that `po4a po4a-booster-music.cfg` has been run first"
                 .format(translations_dir))

    for language in os.listdir(translations_dir):
        print("language " + language)
        make_dir(info_output_dir + '/' + language)

        execute_command("pandoc -r markdown -w html \"{0}/{1}/{3}.md\" -o \"{2}/{1}/{3}.html\""
                        .format(translations_dir, language, info_output_dir, song))


def fix_svg_bug(svg_name):
    search_key = "style=\"font:"

    svg_in = open(svg_name + "001.svg", 'r')
    svg_out = open(svg_name + "fixed.svg", 'w')
    while True:
        svg_line: str = svg_in.readline()
        if not svg_line:
            break
        start = svg_line.find(search_key)
        if start >= 0:
            end = svg_line.find('"', start + len(search_key))
            old_text = svg_line[start:end + 1]
            new_text = ""
            if "italic" in old_text:
                new_text += 'font-style="italic" '
            if "bold" in old_text:
                new_text += 'font-weight="bold" '
            if "Times" in old_text:
                new_text += 'font-family="Times" '
            if "serif" in old_text:
                new_text += 'font-family="serif" '
            if "Helvetica" in old_text:
                new_text += 'font-family="Helvetica" '
            px_end = old_text.find('px')
            if px_end >= 0:
                px_start = px_end - 1
                while old_text[px_start].isnumeric() or old_text[px_start] == '.':
                    px_start -= 1
                font_size = old_text[px_start + 1:px_end]
                new_text += f'font-size="{font_size}px" '
            #print("SVG BUG FIX: replacing '{0}' with '{1}'".format(old_text, new_text))
            svg_line = svg_line.replace(old_text, new_text)
        svg_out.write(svg_line)

    svg_in.close()
    svg_out.close()


def create_book_markdown(src_dir, book_name, song):
    src_path_name = src_dir + '/' + song
    ps_name = src_path_name
    if os.path.isfile(ps_name + "Ps.abc"):
        ps_name += "Ps"
        print("ps_name " + ps_name)

    make_dir(build_web_dir + src_dir)
    web_svg_path_name = build_web_dir + src_dir + "/" + song

    web_md_path_name = build_web_dir + book_name + ".md"

    execute_command("abcm2ps -i -g \"{0}.abc\" -O \"{1}.svg\"".format(ps_name, web_svg_path_name))

    fix_svg_bug(web_svg_path_name)

    title_needed = not os.path.isfile(web_md_path_name)
    out_file = open(web_md_path_name, 'a')
    if out_file == 0:
        sys.exit("Open Error: " + web_md_path_name)

    if title_needed:
        intro_file = open(src_dir + "/Introduction.md", "r")
        out_file.write(intro_file.read())

    in_file = open(src_path_name + ".md", "r")
    if in_file == 0:
        sys.exit("Open Error: " + src_path_name)

    md_text = "\n![]({0}fixed.svg)\n\n".format(src_path_name)
    md_text += in_file.read()
    in_file.close()
    out_file.write(md_text)
    out_file.close()


def create_pandoc_pdf(web_dir, book_name):
    output_dir = build_zip_dir + book_name
    execute_command(
        "cd {0};pandoc --from=markdown+raw_html+yaml_metadata_block '{2}.md' -V geometry:\"top=1.5cm, bottom=1.5cm, left=2cm, right=2cm\" -o '../../{1}/{2}.pdf'"
        .format(web_dir, output_dir, book_name))


def process_songs(src_dir, book_name, all_song_names):
    temp_mma_dir = temp_dir + 'mma/' + src_dir + '/'
    make_dir(temp_mma_dir)
    make_dir(build_zip_dir + book_name + '/' + info_pages + "/en")

    os.system("cp \"{0}/Booster44Lib.mma\" \"{1}\"".format(src_dir, temp_mma_dir))

    song_names = []
    for song in all_song_names:
        if not song.endswith("Ps"):
            song_names.append(song)

    for song in song_names:
        create_midi_files(src_dir, book_name, song, temp_mma_dir)
        create_info_pages(src_dir, book_name, song)
        create_book_markdown(src_dir, book_name, song)

    create_pandoc_pdf(build_web_dir, book_name)


def process_book(src_dir, book_name):
    song_names = find_files_in_dir(src_dir, ".abc")
    print(song_names)
    process_songs(src_dir, book_name, song_names)


def create_zip_file():
    os.system(f'cp "../../doc/images/pianobooster-note-chart.pdf" "{build_dir}/{zip_file_name + music_release}/"')
    rm_file("{0}/{1}.zip".format(build_dir, zip_file_name))
    execute_command("cd {0};zip -r {1}.zip \"{2}\" ".format(build_dir, zip_file_name, zip_file_name + music_release))


def update_translations():
    rm_file("{0}/{1}.zip".format(build_dir, zip_file_name))
    execute_command("po4a po4a-booster-music.cfg")
    if not os.path.isdir(build_translations_dir):
        sys.exit("Error no languages in '{0}'\nEnsure that `po4a po4a-booster-music.cfg` has been run first"
                 .format(build_translations_dir))


def run_main(argv):
    rm_tree(build_web_dir)

    update_translations()
    process_book("BeginnerCourse", "Beginner Course")
    process_book("BoosterMusic", "Booster Music")
    create_zip_file()


if __name__ == "__main__":
    run_main(sys.argv)
