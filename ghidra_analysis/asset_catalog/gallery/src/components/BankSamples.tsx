import type { AudioBankMeta, BankSample, CatalogEntry } from "@/types";
import { SectionHeading } from "./MetaSection";
import { useBankSampleUrl } from "@/data/DataSourceContext";

interface Props {
  bank: AudioBankMeta;
  entry: CatalogEntry;
}

export function BankSamples({ bank, entry }: Props) {
  // The bank's WAV filename — same one used for the parent <audio>.
  // `entry.preview` is set by `payload.ts` to the canonical .wav for
  // an AudioBank.
  const bankFile = entry.preview;
  return (
    <section>
      <SectionHeading>Bank samples</SectionHeading>
      <div className="flex flex-col gap-1.5 rounded-md bg-card/40 px-3 py-2 font-mono text-[10.5px] text-muted-foreground">
        {bank.samples.map(s => (
          <SampleRow key={s.index} sample={s} bankFile={bankFile} />
        ))}
      </div>
    </section>
  );
}

function SampleRow({ sample, bankFile }: { sample: BankSample; bankFile: string | null }) {
  const url = useBankSampleUrl(bankFile, sample.offsetInBank, sample.byteLen);
  return (
    <div className="grid grid-cols-[38px_120px_70px_70px_1fr] items-center gap-2">
      <span className="text-primary">#{sample.index}</span>
      <span className={sample.slug ? "text-foreground" : "italic opacity-60"}>
        {sample.slug ?? "(unnamed)"}
      </span>
      <span>@{sample.offsetInBank.toLocaleString()}</span>
      <span>{sample.byteLen.toLocaleString()} B</span>
      <audio
        controls
        preload="none"
        src={url ?? undefined}
        className="h-6 w-full"
      />
    </div>
  );
}
