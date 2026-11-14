import re
import sys


def chunks(text):
    out = []
    i, n = 0, len(text)
    start = 0
    depth = 0
    paren = 0
    body = None
    while i < n:
        c = text[i]
        if c == '\n' and i + 1 < n and text[i + 1] == '#':
            j = text.find('\n', i + 1)
            i = n if j < 0 else j
            continue
        if i == 0 and c == '#':
            i = text.find('\n')
            continue
        if c in '"\'':
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == '\\' else 1
            i = j + 1
            continue
        if c == '(':
            paren += 1
        elif c == ')':
            paren -= 1
        elif c == '{':
            if depth == 0 and paren == 0 and text[start:i].rstrip().endswith(')'):
                body = i
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0 and body is not None:
                header = text[start:body]
                m = re.search(r'(\w+)\s*\(', re.sub(r'^\s*#.*$', '', header, flags=re.M))
                out.append(('fn', m.group(1) if m else None, header, text[body:i + 1]))
                body = None
                start = i + 1
        elif c == ';' and depth == 0 and paren == 0:
            out.append(('other', text[start:i + 1]))
            start = i + 1
        i += 1
    out.append(('other', text[start:]))
    return out


def main():
    src, arg = sys.argv[1], sys.argv[2]
    exclude = arg.startswith('!')
    names = set() if arg == '-' else set(arg.lstrip('!').split(','))
    text = open(src, encoding='latin1').read()
    pieces = chunks(text)
    keep = lambda name: (name not in names) if exclude else (name in names)
    found = set()
    res = []
    for c in pieces:
        if c[0] == 'fn':
            _, name, header, body = c
            if name in names:
                found.add(name)
            if keep(name):
                res.append(header + body)
            else:
                res.append(header.rstrip() + ';' + '\n' * body.count('\n'))
        else:
            res.append(c[1])
    code = ''.join(c[2] + c[3] for c in pieces if c[0] == 'fn' and keep(c[1]))
    code = re.sub(r'"(\\.|[^"\\])*"|\'(\\.|[^\'\\])*\'', '', code)
    used = set(re.findall(r'\w+', code))
    xcode = ''.join(c[2] + c[3] for c in pieces if c[0] == 'fn' and exclude and not keep(c[1]))
    xcode = re.sub(r'"(\\.|[^"\\])*"|\'(\\.|[^\'\\])*\'', '', xcode)
    xused = set(re.findall(r'\w+', xcode))
    for i, c in enumerate(res):
        v = var_decl(c)
        if not v or arg == '-':
            continue
        static, vars_, decl = v
        found |= vars_ & names
        lines = '\n' * c.count('\n')
        if static and not exclude and not vars_ & (used | names):
            res[i] = lines
        elif static and exclude and vars_ <= xused and not vars_ & used:
            res[i] = lines
        elif not static and not exclude and not vars_ & names:
            res[i] = lines + 'extern ' + decl
    if found != names:
        sys.exit(f'cfunc.py: {src}: no definition of {", ".join(sorted(names - found))}')
    sys.stdout.write(''.join(res))


def var_decl(decl):
    body = re.sub(r'^\s*#.*$', '', decl, flags=re.M).strip()
    if not body.endswith(';') or re.match(r'(extern|typedef)\b', body):
        return None
    parts, cur, depth, init = [], '', 0, False
    for ch in body[:-1]:
        if ch in '([{':
            depth += 1
        elif ch in ')]}':
            depth -= 1
        if depth == 0 and ch == ',':
            parts.append(cur)
            cur, init = '', False
        elif depth == 0 and ch == '=':
            init = True
        elif not init:
            cur += ch
    parts.append(cur)
    out = set()
    for d in parts:
        if '{' in d:
            return None
        if '(' in d:
            m = re.search(r'\(\s*\*+\s*(\w+)', d)
            if not m:
                return None
            out.add(m.group(1))
            continue
        m = re.search(r'(\w+)\s*$', re.sub(r'\[[^\]]*\]', '', d))
        if not m:
            return None
        out.add(m.group(1))
    static = bool(re.match(r'static\b', body))
    return static, out, ','.join(p.strip() for p in parts) + ';'


if __name__ == '__main__':
    main()
