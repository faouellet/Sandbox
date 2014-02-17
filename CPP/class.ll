; ModuleID = 'class.cpp'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%class.Foo = type { float }

@_ZN3FooC1Ev = alias void (%class.Foo*)* @_ZN3FooC2Ev

; Function Attrs: nounwind uwtable
define void @_ZN3FooC2Ev(%class.Foo* %this) unnamed_addr #0 align 2 {
  %1 = alloca %class.Foo*, align 8
  store %class.Foo* %this, %class.Foo** %1, align 8
  %2 = load %class.Foo** %1
  %3 = getelementptr inbounds %class.Foo* %2, i32 0, i32 0
  store float 0.000000e+00, float* %3, align 4
  ret void
}

; Function Attrs: nounwind uwtable
define float @_ZN3Foo3BarEf(%class.Foo* %this, float %val) #0 align 2 {
  %1 = alloca %class.Foo*, align 8
  %2 = alloca float, align 4
  store %class.Foo* %this, %class.Foo** %1, align 8
  store float %val, float* %2, align 4
  %3 = load %class.Foo** %1
  %4 = load float* %2, align 4
  %5 = getelementptr inbounds %class.Foo* %3, i32 0, i32 0
  %6 = load float* %5, align 4
  %7 = fadd float %4, %6
  ret float %7
}

; Function Attrs: nounwind uwtable
define i32 @_ZNK3Foo3BazEv(%class.Foo* %this) #0 align 2 {
  %1 = alloca %class.Foo*, align 8
  store %class.Foo* %this, %class.Foo** %1, align 8
  %2 = load %class.Foo** %1
  ret i32 42
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5 (trunk 199956)"}
