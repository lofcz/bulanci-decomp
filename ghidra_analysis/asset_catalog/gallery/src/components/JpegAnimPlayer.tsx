import { useEffect, useRef, useState } from "react";
import { Play, Pause } from "lucide-react";
import { Button } from "@/components/ui/button";
import { useOverlayUrl, useOverlayUrls } from "@/data/DataSourceContext";

const FPS = 12;

interface Props {
  frames:    string[];
  audioFile: string | null;
}

/** A single transport button drives both the JPEG frame timer and the
 *  audio element; frames stay slaved to `audio.currentTime`.  Cleanup
 *  on unmount stops the interval and any in-flight playback. */
export function JpegAnimPlayer({ frames, audioFile }: Props) {
  const audioRef = useRef<HTMLAudioElement | null>(null);
  const [playing,  setPlaying]  = useState(false);
  const [frameIdx, setFrameIdx] = useState(0);
  const [duration, setDuration] = useState<number | null>(null);
  const [curTime,  setCurTime]  = useState(0);
  const audioSrc  = useOverlayUrl(audioFile) ?? undefined;
  const frameSrcs = useOverlayUrls(frames);

  // Drive frames either off the audio playhead (preferred — syncs to
  // user scrubbing) or off a self-incrementing index if no audio.
  useEffect(() => {
    if (!playing) return;
    const id = window.setInterval(() => {
      if (audioRef.current && audioFile) {
        const t = audioRef.current.currentTime;
        setFrameIdx(Math.floor(t * FPS) % frames.length);
        setCurTime(t);
      } else {
        setFrameIdx(i => (i + 1) % frames.length);
      }
    }, 1000 / FPS);
    return () => window.clearInterval(id);
  }, [playing, frames.length, audioFile]);

  const toggle = () => {
    if (!playing) {
      audioRef.current?.play().catch(() => { /* autoplay blocked */ });
      setPlaying(true);
    } else {
      audioRef.current?.pause();
      setPlaying(false);
    }
  };

  return (
    <div className="flex flex-col items-center gap-3">
      <img
        src={frameSrcs[frameIdx] || undefined}
        alt={`frame ${frameIdx + 1}`}
        className="max-h-[280px] max-w-full rounded-md border border-border"
      />
      <div className="flex flex-wrap items-center gap-3 text-xs text-muted-foreground">
        <Button
          variant="default"
          size="sm"
          onClick={toggle}
          className="font-semibold"
        >
          {playing
            ? <><Pause className="size-3" /> pause</>
            : <><Play  className="size-3" /> play</>}
        </Button>
        <span className="font-mono">
          frame <span className="text-primary font-semibold">{frameIdx + 1}</span>
          {" / "}
          {frames.length}
        </span>
        {audioFile && (
          <span className="font-mono">
            <span className="text-primary font-semibold">{fmtClock(curTime)}</span>
            {" / "}
            {duration != null ? fmtClock(duration) : "…"}
          </span>
        )}
      </div>
      {audioSrc && (
        <audio
          ref={audioRef}
          src={audioSrc}
          preload="metadata"
          onLoadedMetadata={(ev) => setDuration(ev.currentTarget.duration)}
          onPlay={() => setPlaying(true)}
          onPause={() => setPlaying(false)}
          onEnded={() => { setPlaying(false); setFrameIdx(0); setCurTime(0); }}
        />
      )}
    </div>
  );
}

function fmtClock(secs: number): string {
  if (!Number.isFinite(secs)) return "0:00";
  const m = Math.floor(secs / 60);
  const s = Math.floor(secs - m * 60);
  return `${m}:${String(s).padStart(2, "0")}`;
}
