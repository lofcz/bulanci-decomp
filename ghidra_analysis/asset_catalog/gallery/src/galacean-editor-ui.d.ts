// Ambient declaration for `@galacean/editor-ui`.
//
// The published 1.2.0 tarball ships only compiled JS (`dist/es`, `dist/cjs`) —
// its `types/` directory (referenced by package.json) was left out of the
// release, so TypeScript can't resolve any declarations. Until we vendor
// generated `.d.ts` from the source repo (GitHub/editor-ui, `npm run
// build:types`) this keeps the editor-ui control kit usable from TS. Runtime
// resolution via Vite is unaffected; this only satisfies the type checker.
declare module "@galacean/editor-ui";
declare module "@galacean/editor-ui/design-system";
