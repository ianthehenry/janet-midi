#include <janet.h>
#include <functional>
#include <RtMidi.h>
#include <unistd.h>

#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )

unsigned char control(int byte, unsigned char channel) {
  return (unsigned char)((byte << 4) | channel);
}

unsigned char get_note(Janet *argv, int32_t i) {
  int32_t note = janet_getnat(argv, i);
  if (note > 127) {
    janet_panicf("note must fall between 0 and 127, got %d", note);
  }
  return (unsigned char)note;
}

unsigned char get_channel(Janet *argv, int32_t i) {
  int32_t channel = janet_getnat(argv, i);
  if (channel > 15) {
    janet_panicf("channel must fall between 0 and 15, got %d", channel);
  }
  return (unsigned char)channel;
}

unsigned char get_velocity(Janet *argv, int32_t i) {
  int32_t velocity = janet_getnat(argv, i);
  if (velocity > 127) {
    janet_panicf("velocity must fall between 0 and 127, got %d", velocity);
  }
  return (unsigned char)velocity;
}

[[ noreturn ]] static void raise(RtMidiError &error) {
  auto str = error.getMessage();
  janet_panic(str.c_str());
}

static void send(RtMidiOut *midi_out, unsigned char *message, int32_t count) {
  try {
    midi_out->sendMessage(message, count);
  } catch (RtMidiError &error) {
    raise(error);
  }
}

static Janet cfun_virtual_out(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 0);
  try {
    RtMidiOut *midi_out = new RtMidiOut();
    midi_out->openVirtualPort();
    return janet_wrap_pointer(midi_out);
  } catch (RtMidiError &error) {
    raise(error);
  }
}

static Janet cfun_note_on(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 4);
  RtMidiOut *midi_out = (RtMidiOut *)janet_getpointer(argv, 0);
  unsigned char channel = get_channel(argv, 1);
  unsigned char note = get_note(argv, 2);
  unsigned char velocity = get_note(argv, 3);
  unsigned char message[3] = {control(9, channel), note, velocity};
  send(midi_out, message, 3);
  return janet_wrap_nil();
}

static Janet cfun_note_off(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 4);
  RtMidiOut *midi_out = (RtMidiOut *)janet_getpointer(argv, 0);
  unsigned char channel = get_channel(argv, 1);
  unsigned char note = get_note(argv, 2);
  unsigned char velocity = get_note(argv, 3);
  unsigned char message[3] = {control(8, channel), note, velocity};
  send(midi_out, message, 3);
  return janet_wrap_nil();
}

static Janet cfun_set_volume(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 3);
  RtMidiOut *midi_out = (RtMidiOut *)janet_getpointer(argv, 0);
  unsigned char channel = get_channel(argv, 1);
  unsigned char volume = get_note(argv, 2);
  unsigned char message[3] = {control(0xB, channel), 7, volume};
  send(midi_out, message, 3);
  return janet_wrap_nil();
}

static const JanetReg cfuns[] = {
  {"open-virtual-output", cfun_virtual_out, "(open-virtual-output)"},
  {"set-volume", cfun_set_volume, "(set-volume port channel volume)"},
  {"note-on", cfun_note_on, "(note-on port channel note velocity)"},
  {"note-off", cfun_note_off, "(note-off port channel note velocity)"},
  {NULL, NULL, NULL}
};

JANET_MODULE_ENTRY(JanetTable *env) {
  janet_cfuns(env, "midi", cfuns);
}
