# janet-midi

[Janet](https://janet-lang.org) bindings to [RtMidi](http://www.music.mcgill.ca/~gary/rtmidi/), a cross-platform MIDI library.

The bindings are not complete, because my only use-case is defining virtual software instruments in Janet. As such there is no way to receive MIDI input, or to output MIDI to actual connected devices. It wouldn't be hard to add these bindings, though, so please get in touch if you want to do something more ambitious.

This library is a direct port of a pretty low-level API. If you actually want to make music in Janet, you will want to write some higher-level helpers!

# Api

```janet
(open-virtual-output)
(set-volume port channel volume)
(note-on port channel note velocity)
(note-off port channel note velocity)
```

# Example

Open GarageBand or some other DAW, and then run this script:

```janet
(import midi)

(def port (midi/open-virtual-output))

# give your DAW some time to detect the virtual output...
(os/sleep 1)

(def bpm 120)
(def note-duration (/ 60 bpm))

(defn rest []
  (os/sleep note-duration))

(defn play [note]
  (midi/note-on port 0 note 64)
  (os/sleep note-duration)
  (midi/note-off port 0 note 64))

(var note 60)
(def major-scale [2 2 1 2 2 2 1])

(play note)
(each interval major-scale
  (+= note interval)
  (play note))

(rest)

(play note)
(each interval (reverse major-scale)
  (-= note interval)
  (play note))
```

Only tested on macOS, but should work on [Windows or Linux](http://www.music.mcgill.ca/~gary/rtmidi/index.html#compiling) with the right flags.
