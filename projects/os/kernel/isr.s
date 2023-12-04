bits 32

extern ISREntry

%macro ISRWithoutErrorCode 1
global ISR%1
ISR%1:
    push 0 ; dummy error code
    push %1
    jmp ISRsCommon
%endmacro

%macro ISRWithErrorCode 1
global ISR%1
ISR%1:
    push %1
    jmp ISRsCommon
%endmacro

ISRsCommon:
    pusha ; push all general purpose registers

    xor eax, eax
    mov ax, ds
    push eax ; save the currently active data segment on the stack

    mov ax, 0x10 ; 32-bit data segment offset in the GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call ISREntry
    add esp, 4

    pop eax
    mov ds, ax ; restore the previously active data segment
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8 ; discard error code and interrupt number pushed by
               ; the individual interrupt handlers
    iret

ISRWithoutErrorCode 0
ISRWithoutErrorCode 1
ISRWithoutErrorCode 2
ISRWithoutErrorCode 3
ISRWithoutErrorCode 4
ISRWithoutErrorCode 5
ISRWithoutErrorCode 6
ISRWithoutErrorCode 7
ISRWithErrorCode 8
ISRWithoutErrorCode 9
ISRWithErrorCode 10
ISRWithErrorCode 11
ISRWithErrorCode 12
ISRWithErrorCode 13
ISRWithErrorCode 14
ISRWithoutErrorCode 15
ISRWithoutErrorCode 16
ISRWithErrorCode 17
ISRWithoutErrorCode 18
ISRWithoutErrorCode 19
ISRWithoutErrorCode 20
ISRWithErrorCode 21
ISRWithoutErrorCode 22
ISRWithoutErrorCode 23
ISRWithoutErrorCode 24
ISRWithoutErrorCode 25
ISRWithoutErrorCode 26
ISRWithoutErrorCode 27
ISRWithoutErrorCode 28
ISRWithoutErrorCode 29
ISRWithoutErrorCode 30
ISRWithoutErrorCode 31
ISRWithoutErrorCode 32
ISRWithoutErrorCode 33
ISRWithoutErrorCode 34
ISRWithoutErrorCode 35
ISRWithoutErrorCode 36
ISRWithoutErrorCode 37
ISRWithoutErrorCode 38
ISRWithoutErrorCode 39
ISRWithoutErrorCode 40
ISRWithoutErrorCode 41
ISRWithoutErrorCode 42
ISRWithoutErrorCode 43
ISRWithoutErrorCode 44
ISRWithoutErrorCode 45
ISRWithoutErrorCode 46
ISRWithoutErrorCode 47
ISRWithoutErrorCode 48
ISRWithoutErrorCode 49
ISRWithoutErrorCode 50
ISRWithoutErrorCode 51
ISRWithoutErrorCode 52
ISRWithoutErrorCode 53
ISRWithoutErrorCode 54
ISRWithoutErrorCode 55
ISRWithoutErrorCode 56
ISRWithoutErrorCode 57
ISRWithoutErrorCode 58
ISRWithoutErrorCode 59
ISRWithoutErrorCode 60
ISRWithoutErrorCode 61
ISRWithoutErrorCode 62
ISRWithoutErrorCode 63
ISRWithoutErrorCode 64
ISRWithoutErrorCode 65
ISRWithoutErrorCode 66
ISRWithoutErrorCode 67
ISRWithoutErrorCode 68
ISRWithoutErrorCode 69
ISRWithoutErrorCode 70
ISRWithoutErrorCode 71
ISRWithoutErrorCode 72
ISRWithoutErrorCode 73
ISRWithoutErrorCode 74
ISRWithoutErrorCode 75
ISRWithoutErrorCode 76
ISRWithoutErrorCode 77
ISRWithoutErrorCode 78
ISRWithoutErrorCode 79
ISRWithoutErrorCode 80
ISRWithoutErrorCode 81
ISRWithoutErrorCode 82
ISRWithoutErrorCode 83
ISRWithoutErrorCode 84
ISRWithoutErrorCode 85
ISRWithoutErrorCode 86
ISRWithoutErrorCode 87
ISRWithoutErrorCode 88
ISRWithoutErrorCode 89
ISRWithoutErrorCode 90
ISRWithoutErrorCode 91
ISRWithoutErrorCode 92
ISRWithoutErrorCode 93
ISRWithoutErrorCode 94
ISRWithoutErrorCode 95
ISRWithoutErrorCode 96
ISRWithoutErrorCode 97
ISRWithoutErrorCode 98
ISRWithoutErrorCode 99
ISRWithoutErrorCode 100
ISRWithoutErrorCode 101
ISRWithoutErrorCode 102
ISRWithoutErrorCode 103
ISRWithoutErrorCode 104
ISRWithoutErrorCode 105
ISRWithoutErrorCode 106
ISRWithoutErrorCode 107
ISRWithoutErrorCode 108
ISRWithoutErrorCode 109
ISRWithoutErrorCode 110
ISRWithoutErrorCode 111
ISRWithoutErrorCode 112
ISRWithoutErrorCode 113
ISRWithoutErrorCode 114
ISRWithoutErrorCode 115
ISRWithoutErrorCode 116
ISRWithoutErrorCode 117
ISRWithoutErrorCode 118
ISRWithoutErrorCode 119
ISRWithoutErrorCode 120
ISRWithoutErrorCode 121
ISRWithoutErrorCode 122
ISRWithoutErrorCode 123
ISRWithoutErrorCode 124
ISRWithoutErrorCode 125
ISRWithoutErrorCode 126
ISRWithoutErrorCode 127
ISRWithoutErrorCode 128
ISRWithoutErrorCode 129
ISRWithoutErrorCode 130
ISRWithoutErrorCode 131
ISRWithoutErrorCode 132
ISRWithoutErrorCode 133
ISRWithoutErrorCode 134
ISRWithoutErrorCode 135
ISRWithoutErrorCode 136
ISRWithoutErrorCode 137
ISRWithoutErrorCode 138
ISRWithoutErrorCode 139
ISRWithoutErrorCode 140
ISRWithoutErrorCode 141
ISRWithoutErrorCode 142
ISRWithoutErrorCode 143
ISRWithoutErrorCode 144
ISRWithoutErrorCode 145
ISRWithoutErrorCode 146
ISRWithoutErrorCode 147
ISRWithoutErrorCode 148
ISRWithoutErrorCode 149
ISRWithoutErrorCode 150
ISRWithoutErrorCode 151
ISRWithoutErrorCode 152
ISRWithoutErrorCode 153
ISRWithoutErrorCode 154
ISRWithoutErrorCode 155
ISRWithoutErrorCode 156
ISRWithoutErrorCode 157
ISRWithoutErrorCode 158
ISRWithoutErrorCode 159
ISRWithoutErrorCode 160
ISRWithoutErrorCode 161
ISRWithoutErrorCode 162
ISRWithoutErrorCode 163
ISRWithoutErrorCode 164
ISRWithoutErrorCode 165
ISRWithoutErrorCode 166
ISRWithoutErrorCode 167
ISRWithoutErrorCode 168
ISRWithoutErrorCode 169
ISRWithoutErrorCode 170
ISRWithoutErrorCode 171
ISRWithoutErrorCode 172
ISRWithoutErrorCode 173
ISRWithoutErrorCode 174
ISRWithoutErrorCode 175
ISRWithoutErrorCode 176
ISRWithoutErrorCode 177
ISRWithoutErrorCode 178
ISRWithoutErrorCode 179
ISRWithoutErrorCode 180
ISRWithoutErrorCode 181
ISRWithoutErrorCode 182
ISRWithoutErrorCode 183
ISRWithoutErrorCode 184
ISRWithoutErrorCode 185
ISRWithoutErrorCode 186
ISRWithoutErrorCode 187
ISRWithoutErrorCode 188
ISRWithoutErrorCode 189
ISRWithoutErrorCode 190
ISRWithoutErrorCode 191
ISRWithoutErrorCode 192
ISRWithoutErrorCode 193
ISRWithoutErrorCode 194
ISRWithoutErrorCode 195
ISRWithoutErrorCode 196
ISRWithoutErrorCode 197
ISRWithoutErrorCode 198
ISRWithoutErrorCode 199
ISRWithoutErrorCode 200
ISRWithoutErrorCode 201
ISRWithoutErrorCode 202
ISRWithoutErrorCode 203
ISRWithoutErrorCode 204
ISRWithoutErrorCode 205
ISRWithoutErrorCode 206
ISRWithoutErrorCode 207
ISRWithoutErrorCode 208
ISRWithoutErrorCode 209
ISRWithoutErrorCode 210
ISRWithoutErrorCode 211
ISRWithoutErrorCode 212
ISRWithoutErrorCode 213
ISRWithoutErrorCode 214
ISRWithoutErrorCode 215
ISRWithoutErrorCode 216
ISRWithoutErrorCode 217
ISRWithoutErrorCode 218
ISRWithoutErrorCode 219
ISRWithoutErrorCode 220
ISRWithoutErrorCode 221
ISRWithoutErrorCode 222
ISRWithoutErrorCode 223
ISRWithoutErrorCode 224
ISRWithoutErrorCode 225
ISRWithoutErrorCode 226
ISRWithoutErrorCode 227
ISRWithoutErrorCode 228
ISRWithoutErrorCode 229
ISRWithoutErrorCode 230
ISRWithoutErrorCode 231
ISRWithoutErrorCode 232
ISRWithoutErrorCode 233
ISRWithoutErrorCode 234
ISRWithoutErrorCode 235
ISRWithoutErrorCode 236
ISRWithoutErrorCode 237
ISRWithoutErrorCode 238
ISRWithoutErrorCode 239
ISRWithoutErrorCode 240
ISRWithoutErrorCode 241
ISRWithoutErrorCode 242
ISRWithoutErrorCode 243
ISRWithoutErrorCode 244
ISRWithoutErrorCode 245
ISRWithoutErrorCode 246
ISRWithoutErrorCode 247
ISRWithoutErrorCode 248
ISRWithoutErrorCode 249
ISRWithoutErrorCode 250
ISRWithoutErrorCode 251
ISRWithoutErrorCode 252
ISRWithoutErrorCode 253
ISRWithoutErrorCode 254
ISRWithoutErrorCode 255