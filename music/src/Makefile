dirs:= BeginnerCourse BoosterMusic
output:= ../../doc/courses
ABCs:=$(foreach dir, $(dirs), $(wildcard $(dir)/??-*.abc))
MMAs:=$(sort $(foreach dir, $(dirs), $(wildcard $(dir)/??-*.mma)))
MIDs:=$(foreach mma, $(MMAs:.mma=.mid), $(output)/$(mma))
SOLs:=$(foreach abc, $(ABCs:.abc=.solo.mid), $(abc))
SVGs:=$(foreach abc, $(ABCs:.abc=.svg), $(output)/$(abc))
MDs=:$(ABCs:.abc=_en.md)

all: $(output)/README.html

%.html: %.md
	markdown $< > $@

$(output)/README.md: $(SVGs) $(MIDs)
	cat music.md.start > $@
	echo "" >> $@
	sh -c '$(foreach mma, $(MMAs), \
			echo "## $(basename $(notdir $(mma)))"; \
			echo ""; \
			echo "![score]($(basename $(mma)).svg)"; \
			echo ""; \
			[ -f "$(basename $(mma))_en.md" ] && cat "$(basename $(mma))_en.md" || true ; \
			echo ""; \
			echo "[midi file]($(basename $(mma)).mid)" ; \
			echo ""; \
			)' >> "$@"
	cat music.md.end >> $@

$(output)/%.svg: %.abc
	$(foreach dir, $(dirs), mkdir -p $(output)/$(dir);)
	abcm2ps -g -s 0.9 -c "$<" -O "$(output)/$(basename $<).svg"
	mv -f "$(output)/$(basename $<)001.svg" "$(output)/$(basename $<).svg"

%.solo.mid: %.abc
	abc2midi "$<" -o "$@" -RS

$(output)/%.mid: %.mma $(SOLs)
	$(foreach dir, $(dirs), mkdir -p $(output)/$(dir);)
	sh -c 'cd $(dir $<); mma "$(notdir $<)" -o -f "../$@"'

clean:
	rm -f $(SOLs) $(wildcard $(output)/*/*.svg)

cleanall: clean
	rm -fr $(output)
