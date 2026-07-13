import { readFileSync, writeFileSync } from 'fs';
import { Resvg } from '@resvg/resvg-js';

const svg = readFileSync('./icon.svg', 'utf-8');
const resvg = new Resvg(svg, {
  fitTo: { mode: 'width', value: 256 },
});
const pngData = resvg.render();
const pngBuffer = pngData.asPng();
writeFileSync('./icon.png', pngBuffer);
console.log('icon.png generated successfully (' + pngBuffer.length + ' bytes)');
